
#include "GUI/GuildApplyWindow.h"
#include "GUI/MessageBoxWindow.h"

#include "Elements/CustomItemListbox.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include <iostream>

#include <boost/algorithm/string.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

GuildApplyWindow::GuildApplyWindow() :
processing_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("guildapplywindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&GuildApplyWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&GuildApplyWindow::HandleHidden, this));

		closeButton_ = static_cast<PushButton*>(window_->getChild("GuildApply/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildApplyWindow::HandleCloseButtonClicked, this));

		guildList_ = static_cast<CustomItemListbox*>(window_->getChild("GuildApply/GuildList"));

		applyButton_ = static_cast<PushButton*>(window_->getChild("GuildApply/ApplyButton"));
		applyButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildApplyWindow::HandleApplyButtonClicked, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "GuildApplyWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

GuildApplyWindow::~GuildApplyWindow()
{
	window_->destroy();
}

bool GuildApplyWindow::HandleShown(const EventArgs& e)
{
	window_->setModalState(true);
	return true;
}

bool GuildApplyWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

void GuildApplyWindow::HandleGuildList(GamePacket& packet)
{
	guildList_->resetList();
	processing_ = false;
	processGuildItem_ = nullptr;

	sf::Uint16 count;
	packet >> count;

	if (count == 0)
	{
		Game::Instance().messageBoxWindow->Show("There are no guilds available.");
	}
	else
	{
		for (unsigned int i = 0; i < count; ++i)
		{
			std::string name;
			packet >> name;
			boost::to_upper(name);

			ItemEntry* guildItem = static_cast<ItemEntry*>(WindowManager::getSingleton().createWindow("Soma/ListboxItem"));
			guildItem->subscribeEvent(ItemEntry::EventMouseEnters,
				Event::Subscriber(&GuildApplyWindow::HandleGuildListItemMouseEnters, this));
			guildItem->setText(name);

			guildList_->addItem(guildItem);
		}

		guildList_->sortList();

		window_->show();
	}
}

bool GuildApplyWindow::HandleCloseButtonClicked(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool GuildApplyWindow::HandleApplyButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		ItemEntry* guildItem = guildList_->getFirstSelectedItem();
		if (guildItem != nullptr)
		{
			processing_ = true;
			processGuildItem_ = guildItem;

			GamePacket packet(GameProtocol::PKT_GUILD_REQ);
			packet << guildItem->getText().c_str();
			Game::Instance().gameSocket->Send(packet);
		}
	}
	return true;
}

bool GuildApplyWindow::HandleGuildListItemMouseEnters(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	ItemEntry* guildItem = static_cast<ItemEntry*>(mouseEventArgs.window);
	if (guildItem != nullptr && guildItem->getTooltipText().empty())
	{
		processing_ = true;
		processGuildItem_ = guildItem;

		GamePacket packet(GameProtocol::PKT_GUILD_INFO);
		packet << guildItem->getText().c_str();
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

void GuildApplyWindow::HandleRequestGuildInfo(GamePacket& packet)
{
	processing_ = false;

	std::string info;
	packet >> info;

	if (!info.empty())
	{
		processGuildItem_->setTooltipText(info);
	}
	else
	{
		processGuildItem_->setTooltipText("No description given");
	}
}

void GuildApplyWindow::HandleApplyToGuild(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		Game::Instance().messageBoxWindow->Show("Your join request has been submitted.");
	}
	else
	{
		sf::Uint8 errorCode;
		packet >> errorCode;

		switch (errorCode)
		{
		case GameProtocol::GUILD_ABSENT_GUILD_NAME:
			Game::Instance().messageBoxWindow->Show("This Guild does not exist.");
			break;
		case GameProtocol::GUILD_ALREADY_JOIN:
			Game::Instance().messageBoxWindow->Show("You're already in a guild.");
			break;
		case GameProtocol::GUILD_ALREADY_JOIN_REQ:
			Game::Instance().messageBoxWindow->Show("You already submitted a request to join a Guild.");
			break;
		default:
			Game::Instance().messageBoxWindow->Show("You cannot currently submit a join request. Try again later.");
			break;
		}
	}
	window_->hide();
}

} // namespace GUI