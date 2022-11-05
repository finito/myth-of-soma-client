
#include "GUI/GuildMenu.h"
#include "GUI/MessageBoxWindow.h"
#include "GUI/GuildInfoWindow.h"

#include "Components/AttributeComponent.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include <boost/format.hpp>
#include <iostream>
#include <cctype>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

GuildMenu::GuildMenu() :
processing_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("guildmenu.layout");
		window_->hide();
		window_->setAlwaysOnTop(true);
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&GuildMenu::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&GuildMenu::HandleHidden, this));
		window_->subscribeEvent(Window::EventMouseLeavesArea, Event::Subscriber(&GuildMenu::HandleMouseLeavesArea, this));

		layout_ = static_cast<SequentialLayoutContainer*>(window_->getChild("GuildMenu/LayoutContainer"));

		applicationButton_ = static_cast<PushButton*>(window_->getChild("GuildMenu/ApplicationButton"));
		applicationButton_->hide();
		applicationButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildMenu::HandleApplicationButtonClicked, this));

		guildButton_ = static_cast<PushButton*>(window_->getChild("GuildMenu/GuildButton"));
		guildButton_->hide();
		guildButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildMenu::HandleGuildButtonClicked, this));

		makeGuildButton_ = static_cast<PushButton*>(window_->getChild("GuildMenu/MakeGuildButton"));
		makeGuildButton_->hide();
		makeGuildButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildMenu::HandleMakeGuildButtonClicked, this));

		applyToGuildButton_ = static_cast<PushButton*>(window_->getChild("GuildMenu/ApplyToGuildButton"));
		applyToGuildButton_->hide();
		applyToGuildButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildMenu::HandleApplyToGuildButtonClicked, this));

		cancelApplyButton_ = static_cast<PushButton*>(window_->getChild("GuildMenu/CancelApplyButton"));
		cancelApplyButton_->hide();
		cancelApplyButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildMenu::HandleCancelApplyButtonClicked, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "GuildMenu: Failed to initialize: " << e.what() << std::endl;
	}
}

GuildMenu::~GuildMenu()
{
	window_->destroy();
}

bool GuildMenu::HandleShown(const EventArgs& e)
{
	window_->setModalState(true);
	return true;
}

bool GuildMenu::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

void GuildMenu::HandleGuildOpen(GamePacket& packet, sf::Vector2i position)
{
	layout_->removeChildWindow(applicationButton_);
	layout_->removeChildWindow(guildButton_);
	layout_->removeChildWindow(makeGuildButton_);
	layout_->removeChildWindow(applyToGuildButton_);
	layout_->removeChildWindow(cancelApplyButton_);

	sf::Uint8 menuType;
	packet >> menuType;

	switch (menuType)
	{
	case 0:
		packet >> guildMoney_;
		packet >> guildLevel_;
		packet >> guildName_;
		applied_ = !guildName_.empty();

		layout_->addChildWindow(makeGuildButton_);
		makeGuildButton_->show();

		if (applied_)
		{
			layout_->addChildWindow(cancelApplyButton_);
			cancelApplyButton_->show();
		}
		else
		{
			layout_->addChildWindow(applyToGuildButton_);
			applyToGuildButton_->show();
		}
		break;
	case 1: // Fall-through!
	case 4:
		layout_->addChildWindow(guildButton_);
		guildButton_->show();
		break;
	case 2: // Fall-through!
	case 3:
		layout_->addChildWindow(applicationButton_);
		layout_->addChildWindow(guildButton_);
		applicationButton_->show();
		guildButton_->show();
		break;
	}

	layout_->layout();
	window_->setSize(layout_->getSize());

	UVector2 centredPosition(
		UDim(0, position.x - window_->getPixelSize().d_width  / 2),
		UDim(0, position.y - window_->getPixelSize().d_height / 2));

	window_->setPosition(centredPosition);
	window_->show();
}

void GuildMenu::HandleCancelApplyGuild(GamePacket& packet)
{
	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		Game::Instance().messageBoxWindow->Show("You successfully cancelled your join request.");
	}
	else
	{

		sf::Uint8 errorCode;
		packet >> errorCode;

		switch (errorCode)
		{
		case GameProtocol::GUILD_ABSENT_JOIN_REQ:
			Game::Instance().messageBoxWindow->Show("Join request has not been submitted.");
			break;
		case GameProtocol::GUILD_ABSENT_GUILD_NAME:
			Game::Instance().messageBoxWindow->Show("This Guild does not exist.");
			break;
		case GameProtocol::GUILD_ALREADY_JOIN:
			Game::Instance().messageBoxWindow->Show("You're already in a guild.");
			break;
		case GameProtocol::GUILD_ABSENT_REQ_USER: // Fall-through!
		default:
			Game::Instance().messageBoxWindow->Show("Requesting player does not exist in the current list.");
			break;
		}
	}
}

bool GuildMenu::HandleApplicationButtonClicked(const EventArgs& e)
{
	GamePacket packet(GameProtocol::PKT_GUILD_APPLICATION_LIST);
	Game::Instance().gameSocket->Send(packet);
	window_->hide();
	return true;
}

bool GuildMenu::HandleGuildButtonClicked(const EventArgs& e)
{
	GamePacket packet(GameProtocol::PKT_GUILD_MEMBER_LIST);
	Game::Instance().gameSocket->Send(packet);
	window_->hide();
	return true;
}

bool GuildMenu::HandleMakeGuildButtonClicked(const EventArgs& e)
{
	if (!applied_)
	{
		Game::Instance().messageBoxWindow->Show(
			boost::str(boost::format("To set up a guild, %1% Barr is needed.\nDo you want to continue?") % guildMoney_),
			MessageBoxMode::OKCancel, Event::Subscriber(&GuildMenu::HandleMakeGuildMessageBoxResult, this));
	}
	else
	{
		Game::Instance().messageBoxWindow->Show("You already submitted a request to join a Guild.");
	}
	return true;
}

bool GuildMenu::HandleApplyToGuildButtonClicked(const EventArgs& e)
{
	GamePacket packet(GameProtocol::PKT_GUILD_LIST);
	Game::Instance().gameSocket->Send(packet);
	window_->hide();
	return true;
}

bool GuildMenu::HandleCancelApplyButtonClicked(const EventArgs& e)
{
	Game::Instance().messageBoxWindow->Show("Do you really want to delete the joining request to " + guildName_ + " Guild?",
		MessageBoxMode::OKCancel, Event::Subscriber(&GuildMenu::HandleCancelApplyMessageBoxResult, this));
	return true;
}

bool GuildMenu::HandleCancelApplyMessageBoxResult(const EventArgs& e)
{
	if (Game::Instance().messageBoxWindow->GetResult() == MessageBoxResult::OK)
	{
		GamePacket packet(GameProtocol::PKT_GUILD_REMOVE_REQ);
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

bool GuildMenu::HandleMouseLeavesArea(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool GuildMenu::HandleMakeGuildMessageBoxResult(const EventArgs& e)
{	
	if (Game::Instance().messageBoxWindow->GetResult() != MessageBoxResult::OK)
	{
		return true;
	}

	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent == nullptr)
	{
		return true;
	}

	if (attributeComponent->GetMoney() < guildMoney_)
	{
		Game::Instance().messageBoxWindow->Show("You need more money to create a Guild.");
	}
	else if (attributeComponent->GetLevel() < guildLevel_)
	{
		Game::Instance().messageBoxWindow->Show(
			boost::str(boost::format("The minimum level required to create the guild is %1%.") % guildLevel_));
	}
	else
	{
		window_->hide();

		Game::Instance().guildInfoWindow->Show("", "", false, Event::Subscriber(&GuildMenu::HandleGuildInfoResult, this));
	}
	return true;
}

bool GuildMenu::HandleGuildInfoResult(const EventArgs& e)
{
	if (Game::Instance().guildInfoWindow->GetResult() != GuildInfoResult::OK)
	{
		return true;
	}

	const std::string name = Game::Instance().guildInfoWindow->GetName().c_str();
	if (!IsGuildNameValid(name))
	{
		Game::Instance().messageBoxWindow->Show("Guild name is not valid.\nUse another name.");
		return true;
	}

	processing_ = true;

	GamePacket packet(GameProtocol::PKT_GUILD_NEW);
	packet << name << Game::Instance().guildInfoWindow->GetText().c_str();
	Game::Instance().gameSocket->Send(packet);
	return true;
}

bool GuildMenu::IsGuildNameValid(const std::string& name)
{
	for (size_t i = 0; i < name.length(); i++)
	{
		if (!std::isalnum(name[i]) && name[i] != '_')
		{
			return false;
		}
	}
	return true;
}

void GuildMenu::HandleMakeGuild(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		Game::Instance().messageBoxWindow->Show("Congratulations.\nYou successfully created a Guild.");
	}
	else
	{
		sf::Uint8 errorCode;
		packet >> errorCode;

		switch (errorCode)
		{
		case GameProtocol::GUILD_ALREADY_JOIN:
			Game::Instance().messageBoxWindow->Show("You're already in a guild.");
			break;
		case GameProtocol::GUILD_SMALL_MONEY:
			Game::Instance().messageBoxWindow->Show("You need more money to create a Guild.");
			break;
		case GameProtocol::GUILD_SMALL_LEVEL:
			Game::Instance().messageBoxWindow->Show("The minimum level required to create the guild is %d.");
			break;
		case GameProtocol::GUILD_INVALID_GUILD_NAME:
			Game::Instance().messageBoxWindow->Show("Guild name is not valid.\nUse another name.");
			break;
		case GameProtocol::GUILD_INVALID_GUILD_INFO:
			Game::Instance().messageBoxWindow->Show("Guild informations are not valid.\nWrite them again.");
			break;
		case GameProtocol::GUILD_SAME_GUILD_NAME:
			Game::Instance().messageBoxWindow->Show("This name is already used by another guild.\nPlease change it.");
			break;
		case GameProtocol::GUILD_FULL:
			Game::Instance().messageBoxWindow->Show("The maximum number of guild has been reached. Please wait before making a new one.");
			break;
		default:
			Game::Instance().messageBoxWindow->Show("Due to technical problem, you cannot create the Guild now. Try again later");
			break;
		}
	}
	window_->hide();
}

} // namespace GUI