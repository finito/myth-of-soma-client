
#include "GUI/ExchangeTraderWindow.h"
#include "GUI/InputBoxWindow.h"
#include "GUI/Common/Item.h"

#include "Components/AttributeComponent.h"
#include "Components/ItemAttributeComponent.h"

#include "Network/GameProtocol.h"
#include "Network/GamePacket.h"
#include "Network/GameSocket.h"

#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

ExchangeTraderWindow::ExchangeTraderWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("exchangetraderwindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&ExchangeTraderWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&ExchangeTraderWindow::HandleHidden, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&ExchangeTraderWindow::HandleScrollShop, this));

		closeButton_ = static_cast<PushButton*>(window_->getChild("ExchangeTrader/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ExchangeTraderWindow::HandleClose, this));

		scrollbar_ = static_cast<Scrollbar*>(window_->getChild("ExchangeTrader/Scrollbar"));
		scrollbar_->setStepSize(2.0f);
		scrollbar_->setPageSize(static_cast<float>(SLOT_COUNT));
		scrollbar_->setOverlapSize(0);
		scrollbar_->setDocumentSize(0);
		scrollbar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&ExchangeTraderWindow::HandleScrollPositionChanged, this));

		inventoryMoney_ = window_->getChild("ExchangeTrader/InventoryMoney");

		for (int i = 0; i < SLOT_COUNT; ++i)
		{
			slots_[i] = window_->getChild("ExchangeTrader/Slot" + PropertyHelper::intToString(i+1));
			slots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&ExchangeTraderWindow::HandleMouseEnterSlotArea, this));
			slots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&ExchangeTraderWindow::HandleMouseLeaveSlotArea, this));
			slots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&ExchangeTraderWindow::HandleScrollShop, this));
			slots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&ExchangeTraderWindow::HandleSlotMouseClick, this));

			names_[i] = window_->getChild("ExchangeTrader/Name" + PropertyHelper::intToString(i+1));
		}

		System::getSingleton().getGUISheet()->addChildWindow(window_);

		buyWindow_ = WindowManager::getSingleton().loadWindowLayout("exchangetraderbuywindow.layout");
		buyWindow_->hide();

		cancelButton_ = static_cast<PushButton*>(buyWindow_->getChild("ExchangeTraderBuy/CancelButton"));
		cancelButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ExchangeTraderWindow::HandleCloseBuy, this));

		buyButton_ = static_cast<PushButton*>(buyWindow_->getChild("ExchangeTraderBuy/BuyButton"));
		buyButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ExchangeTraderWindow::HandleBuyButtonClicked, this));

		cost_ = buyWindow_->getChild("ExchangeTraderBuy/Cost");

		buySlot_ = buyWindow_->getChild("ExchangeTraderBuy/Slot");
		buySlot_->subscribeEvent(Window::EventMouseEntersArea,
			Event::Subscriber(&ExchangeTraderWindow::HandleMouseEnterSlotArea, this));
		buySlot_->subscribeEvent(Window::EventMouseLeavesArea,
			Event::Subscriber(&ExchangeTraderWindow::HandleMouseLeaveSlotArea, this));

		required_ = buyWindow_->getChild("ExchangeTraderBuy/Required");

		name_ = buyWindow_->getChild("ExchangeTraderBuy/Name");

		System::getSingleton().getGUISheet()->addChildWindow(buyWindow_);
	}
	catch (Exception& e)
	{
		std::cerr << "ExchangeTraderWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

ExchangeTraderWindow::~ExchangeTraderWindow()
{
	window_->destroy();
}

bool ExchangeTraderWindow::HandleShown(const EventArgs& e)
{
	scrollbar_->setScrollPosition(0);
	window_->moveToFront();
	window_->setModalState(true);
	Update();
	return true;
}

bool ExchangeTraderWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool ExchangeTraderWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

void ExchangeTraderWindow::Update()
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent == nullptr)
	{
		return;
	}

	inventoryMoney_->setText(PropertyHelper::uintToString(attributeComponent->GetMoney()));

	float scrollPosition = scrollbar_->getScrollPosition();
	float stepSize	     = scrollbar_->getStepSize();

	unsigned int startPosition = static_cast<unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < items_.size())
		{
			setItemSlot(*slots_[i], &items_[index]);
			names_[i]->setText(getItemName(items_[index]));
		}
		else
		{
			setItemSlot(*slots_[i], nullptr);
			names_[i]->setText("");
		}
	}
}

bool ExchangeTraderWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool ExchangeTraderWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	return handleMouseEnterItemSlotArea(nullptr, nullptr, e);
}

bool ExchangeTraderWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

void ExchangeTraderWindow::HandleOpenExchangeTrader(GamePacket& packet)
{
	sf::Uint16 itemCount;
	packet >> itemCount;

	ItemArray().swap(items_);
	items_.resize(itemCount);
	for (unsigned int i = 0; i < itemCount; ++i)
	{
		items_[i].HandleItemReceive(packet, GameProtocol::ITEM_AUCTION);
		if (items_[i].GetDura() == 0)
		{
			items_[i].SetDura(1);
		}
	}

	const float scrollPosition = scrollbar_->getScrollPosition();
	const float stepSize	   = scrollbar_->getStepSize();
	const float documentSize   = stepSize * std::ceilf(items_.size() / stepSize);

	scrollbar_->setDocumentSize(documentSize);
	scrollbar_->setScrollPosition(std::min(scrollPosition, documentSize));

	window_->show();
}

bool ExchangeTraderWindow::HandleScrollShop(const EventArgs& e)
{
	return handleItemScroll(*scrollbar_, e, "ExchangeTrader/Slot");
}

bool ExchangeTraderWindow::HandleSlotMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(mouseEventArgs.window->getUserData());
		if (item != nullptr && !item->Empty())
		{
			setItemSlot(*buySlot_, item);
			cost_->setText(PropertyHelper::uintToString(item->GetMoney()));
			name_->setText(getItemName(*item));
			required_->setText("");
			for (int i = 0; i < ItemAttributeComponent::REQUIRED_ITEM_COUNT; ++i)
			{
				if (!item->GetRequiredItemName(i).empty())
				{
					required_->appendText(item->GetRequiredItemName(i) + " : " + PropertyHelper::uintToString(item->GetRequiredItemQuantity(i)) + "\n");
				}
			}
			buyWindow_->moveToFront();
			buyWindow_->setModalState(true);
			buyWindow_->show();
		}
	}
	return true;
}

void ExchangeTraderWindow::HideBuyWindow()
{
	buyWindow_->setModalState(false);
	buyWindow_->hide();
}

bool ExchangeTraderWindow::HandleCloseBuy(const EventArgs& e)
{
	HideBuyWindow();
	return true;
}

bool ExchangeTraderWindow::HandleBuyButtonClicked(const EventArgs& e)
{
	const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(buySlot_->getUserData());
	if (item != nullptr && !item->Empty())
	{
		if (item->GetQuantity() > 1)
		{
			Game::Instance().inputBoxWindow->Show("How Many?", Event::Subscriber(&ExchangeTraderWindow::HandleBuyItemInputResult, this));
			Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(item->GetQuantity()));
		}
		else
		{
			SendExchangeTraderBuy(item->GetIndex(), 1);
			HideBuyWindow();
		}
	}
	return true;
}

bool ExchangeTraderWindow::HandleBuyItemInputResult(const EventArgs& e)
{
	const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(buySlot_->getUserData());
	if (item != nullptr && !item->Empty())
	{
		const unsigned int howMany = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
		if (howMany > 0 && item->GetQuantity() >= howMany)
		{
			SendExchangeTraderBuy(item->GetIndex(), howMany);
			HideBuyWindow();
		}
	}
	return true;
}

void ExchangeTraderWindow::SendExchangeTraderBuy(const unsigned short index, const unsigned short howMany)
{
	GamePacket packet(GameProtocol::PKT_TRADEREXCHANGE);
	packet << static_cast<sf::Uint8>(2) // Buy Item
	       << static_cast<sf::Uint16>(index)
	       << static_cast<sf::Uint16>(howMany);
	Game::Instance().gameSocket->Send(packet);
}

} // namespace GUI