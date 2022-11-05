
#include "GUI/ItemTransformWindow.h"
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

ItemTransformWindow::ItemTransformWindow() :
processing_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("itemtransformwindow.layout");
		window_->setAlwaysOnTop(true);
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&ItemTransformWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&ItemTransformWindow::HandleHidden, this));

		closeButton_ = static_cast<PushButton*>(window_->getChild("ItemTransform/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&ItemTransformWindow::HandleCloseButtonClicked, this));

		itemList_ = static_cast<CustomItemListbox*>(window_->getChild("ItemTransform/ItemList"));

		applyButton_ = static_cast<PushButton*>(window_->getChild("ItemTransform/ApplyButton"));
		applyButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&ItemTransformWindow::HandleApplyButtonClicked, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "ItemTransform: Failed to initialize: " << e.what() << std::endl;
	}
}

ItemTransformWindow::~ItemTransformWindow()
{
	window_->destroy();
}

bool ItemTransformWindow::HandleShown(const EventArgs& e)
{
	window_->moveToFront();
	window_->setModalState(true);
	return true;
}

bool ItemTransformWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

void ItemTransformWindow::HandleItemList(GamePacket& packet)
{
	itemList_->resetList();
	processing_ = false;
	processItem_ = nullptr;

	sf::Uint16 count;
	packet >> count;

	if (count > 0)
	{
		for (unsigned int i = 0; i < count; ++i)
		{
			sf::Uint16 itemIndex;
			packet >> itemIndex;

			std::string itemName;
			packet >> itemName;

			ItemEntry* item = static_cast<ItemEntry*>(WindowManager::getSingleton().createWindow("Soma/ListboxItem"));
			item->setID(itemIndex);
			item->setText(itemName);

			itemList_->addItem(item);
		}

		window_->show();
	}
}

bool ItemTransformWindow::HandleCloseButtonClicked(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool ItemTransformWindow::HandleApplyButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		ItemEntry* item = itemList_->getFirstSelectedItem();
		if (item)
		{
			processing_ = true;
			processItem_ = item;

			GamePacket packet(GameProtocol::PKT_CHANGE_OTHER_ITEM);
			packet
				<< static_cast<unsigned char>(1) // Success
				<< static_cast<unsigned short>(item->getID())
				<< static_cast<unsigned short>(1); // Item Quantity
			Game::Instance().gameSocket->Send(packet);
		}
	}
	return true;
}

void ItemTransformWindow::HandleItemTransform(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		std::string itemName;
		packet >> itemName;
		Game::Instance().messageBoxWindow->Show("Successfully transformed into " + itemName + ".");
	}
	else
	{
		Game::Instance().messageBoxWindow->Show("Failed to transform item.");
	}

	window_->setModalState(false);
	window_->hide();
}

} // namespace GUI