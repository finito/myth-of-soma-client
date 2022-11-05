
#include "GUI/GuildJoinWindow.h"
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

GuildJoinWindow::GuildJoinWindow() :
processing_(false),
processApplicationItem_(nullptr)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("guildjoinwindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&GuildJoinWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&GuildJoinWindow::HandleHidden, this));

		closeButton_ = static_cast<PushButton*>(window_->getChild("GuildJoin/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildJoinWindow::HandleCloseButtonClicked, this));

		applicationList_ = static_cast<CustomItemListbox*>(window_->getChild("GuildJoin/ApplicationList"));

		rejectButton_ = static_cast<PushButton*>(window_->getChild("GuildJoin/RejectButton"));
		rejectButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildJoinWindow::HandleRejectButtonClicked, this));

		joinButton_ = static_cast<PushButton*>(window_->getChild("GuildJoin/JoinButton"));
		joinButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildJoinWindow::HandleJoinButtonClicked, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "GuildJoinWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

GuildJoinWindow::~GuildJoinWindow()
{
	window_->destroy();
}

bool GuildJoinWindow::HandleShown(const EventArgs& e)
{
	window_->setModalState(true);
	return true;
}

bool GuildJoinWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

void GuildJoinWindow::HandleApplicationList(GamePacket& packet)
{
	applicationList_->resetList();
	processing_ = false;
	processApplicationItem_ = nullptr;

	sf::Uint16 count;
	packet >> count;

	if (count == 0)
	{
		Game::Instance().messageBoxWindow->Show("Your Join Requests list is currently empty.");
	}
	else
	{
		for (unsigned int i = 0; i < count; ++i)
		{
			std::string name;
			packet >> name;
			boost::to_upper(name);

			ItemEntry* applicationItem = static_cast<ItemEntry*>(WindowManager::getSingleton().createWindow("Soma/ListboxItem"));
			applicationItem->subscribeEvent(ItemEntry::EventMouseEnters,
				Event::Subscriber(&GuildJoinWindow::HandleApplcationListItemMouseEnters, this));
			applicationItem->setText(name);

			applicationList_->addItem(applicationItem);
		}

		window_->show();
	}
}

bool GuildJoinWindow::HandleCloseButtonClicked(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool GuildJoinWindow::HandleRejectButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		ItemEntry* applicationItem = applicationList_->getFirstSelectedItem();
		if (applicationItem)
		{
			processing_ = true;
			processApplicationItem_ = applicationItem;

			GamePacket packet(GameProtocol::PKT_GUILD_MOVE_REJECT);
			packet << applicationItem->getText().c_str();
			Game::Instance().gameSocket->Send(packet);
		}
	}
	return true;
}

bool GuildJoinWindow::HandleJoinButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		ItemEntry* applicationItem = applicationList_->getFirstSelectedItem();
		if (applicationItem)
		{
			processing_ = true;
			processApplicationItem_ = applicationItem;

			GamePacket packet(GameProtocol::PKT_GUILD_MOVE_REQ);
			packet << applicationItem->getText().c_str();
			Game::Instance().gameSocket->Send(packet);
		}
	}
	return true;
}

bool GuildJoinWindow::HandleApplcationListItemMouseEnters(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	ItemEntry* applicationItem = static_cast<ItemEntry*>(mouseEventArgs.window);
	if (applicationItem != nullptr && applicationItem->getTooltipText().empty())
	{
		processing_ = true;
		processApplicationItem_ = applicationItem;

		GamePacket packet(GameProtocol::PKT_GUILD_REQ_USER_INFO);
		packet << applicationItem->getText().c_str();
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

void GuildJoinWindow::HandleRequestPlayerInfo(GamePacket& packet)
{
	processing_ = false;

	std::string info;
	packet >> info;

	if (!info.empty())
	{
		processApplicationItem_->setTooltipText(info);
	}
	else
	{
		HandleApplicationErrorCode(packet);
	}
}

void GuildJoinWindow::HandleApplicationReject(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		try
		{
			applicationList_->removeItem(processApplicationItem_);
		}
		catch (InvalidRequestException)
		{
			// Do not need to do anything further.
		}
	}
	else
	{
		HandleApplicationErrorCode(packet);
	}
}

void GuildJoinWindow::HandleApplicationJoin(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		try
		{
			applicationList_->removeItem(processApplicationItem_);
		}
		catch (InvalidRequestException)
		{
			// Do not need to do anything further.
		}
	}
	else
	{
		HandleApplicationErrorCode(packet);
	}
}

void GuildJoinWindow::HandleApplicationErrorCode(GamePacket& packet)
{
	sf::Uint8 errorCode;
	packet >> errorCode;

	switch (errorCode)
	{
	case GameProtocol::GUILD_NOT_GUILD_USER:
		Game::Instance().messageBoxWindow->Show("You are not one of this Guild's members.");
		break;
	case GameProtocol::GUILD_SMALL_RANK:
		Game::Instance().messageBoxWindow->Show("You do not have the permission.");
		break;
	case GameProtocol::GUILD_ABSENT_JOIN_REQ:
		Game::Instance().messageBoxWindow->Show("Join request has not been submitted.");
		break;
	case GameProtocol::GUILD_ABSENT_GUILD_NAME:
		Game::Instance().messageBoxWindow->Show("This Guild does not exist.");
		break;
	case GameProtocol::GUILD_ABSENT_REQ_USER:
		Game::Instance().messageBoxWindow->Show("Requesting player does not exist in the current list.");
		break;
	case GameProtocol::GUILD_ALREADY_JOIN:
		Game::Instance().messageBoxWindow->Show("You're already in a guild.");
		break;
	default:
		Game::Instance().messageBoxWindow->Show("Due to a system error you cannot currently complete the join request.");
		break;
	}
}

} // namespace GUI