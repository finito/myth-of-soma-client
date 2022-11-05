
#include "GUI/PosShopWindow.h"
#include "GUI/InputBoxWindow.h"
#include "GUI/Common/Item.h"

#include "Components/AttributeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/ItemAttributeComponent.h"

#include "Network/GameProtocol.h"
#include "Network/GamePacket.h"
#include "Network/GameSocket.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

String PosShopWindow::EventInventoryChanged = "EventInventoryChanged";

PosShopWindow::PosShopWindow() :
selectedSlot_(nullptr),
selectedItem_(nullptr),
processing_(false),
inventoryCanScroll_(false),
shopCanScroll_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("posshopwindow.layout");
		window_->hide();
		window_->addEvent(EventInventoryChanged);
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&PosShopWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&PosShopWindow::HandleHidden, this));
		window_->subscribeEvent(EventInventoryChanged, Event::Subscriber(&PosShopWindow::HandleInventoryChanged, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&PosShopWindow::HandleScrollInventory, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&PosShopWindow::HandleScrollShop, this));

		slotSelection_ = window_->getChild("PosShop/SlotSelection");
		window_->removeChildWindow(slotSelection_);

		closeButton_ = static_cast<PushButton*>(window_->getChild("PosShop/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&PosShopWindow::HandleClose, this));

		buyButton_ = static_cast<PushButton*>(window_->getChild("PosShop/BuyButton"));
		buyButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&PosShopWindow::HandleBuyButtonClicked, this));

		inventoryScrollBar_ = static_cast<Scrollbar*>(window_->getChild("PosShop/HandScrollbar"));
		inventoryScrollBar_->setStepSize(2.0f);
		inventoryScrollBar_->setPageSize(static_cast<float>(INVENTORY_SLOT_COUNT));
		inventoryScrollBar_->setOverlapSize(0);
		inventoryScrollBar_->setDocumentSize(0);
		inventoryScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&PosShopWindow::HandleScrollPositionChanged, this));

		shopScrollBar_ = static_cast<Scrollbar*>(window_->getChild("PosShop/StoreScrollbar"));
		shopScrollBar_->setStepSize(2.0f);
		shopScrollBar_->setPageSize(static_cast<float>(SHOP_SLOT_COUNT));
		shopScrollBar_->setOverlapSize(0);
		shopScrollBar_->setDocumentSize(0);
		shopScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&PosShopWindow::HandleScrollPositionChanged, this));

		pos_ = window_->getChild("PosShop/Pos");

		cost_ = window_->getChild("PosShop/Cost");

		for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
		{
			inventorySlots_[i] = window_->getChild("PosShop/HandSlot" + PropertyHelper::intToString(i+1));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&PosShopWindow::HandleMouseEnterSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&PosShopWindow::HandleMouseLeaveSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&PosShopWindow::HandleSlotMouseClick, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&PosShopWindow::HandleScrollInventory, this));
		}

		for (int i = 0; i < SHOP_SLOT_COUNT; ++i)
		{
			shopSlots_[i] = window_->getChild("PosShop/StoreSlot" + PropertyHelper::intToString(i+1));
			shopSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&PosShopWindow::HandleMouseEnterSlotArea, this));
			shopSlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&PosShopWindow::HandleMouseLeaveSlotArea, this));
			shopSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&PosShopWindow::HandleSlotMouseClick, this));
			shopSlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&PosShopWindow::HandleScrollShop, this));
		}

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "PosShopWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

PosShopWindow::~PosShopWindow()
{
	slotSelection_->destroy();
	window_->destroy();
}

bool PosShopWindow::HandleShown(const EventArgs& e)
{
	selectedItem_ = nullptr;
	cost_->setText("");
	inventoryScrollBar_->setScrollPosition(0);
	shopScrollBar_->setScrollPosition(0);
	window_->moveToFront();
	window_->setModalState(true);
	Update();
	return true;
}

bool PosShopWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool PosShopWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

void PosShopWindow::Update()
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (attributeComponent == nullptr || inventoryComponent == nullptr)
	{
		return;
	}

	pos_->setText(PropertyHelper::uintToString(attributeComponent->GetPos()));

	if (selectedSlot_ != nullptr)
	{
		selectedSlot_->removeChildWindow(slotSelection_);
		selectedSlot_ = nullptr;
	}

	ItemPtrArray().swap(inventoryItems_);
	for (unsigned int i = EQUIPMENT_SLOT_COUNT; i < inventoryComponent->GetInventoryItemSize(); ++i)
	{
		if (!inventoryComponent->GetInventoryItem(i).Empty())
		{
			inventoryComponent->GetInventoryItem(i).SetIndex(i);
			inventoryItems_.push_back(&inventoryComponent->GetInventoryItem(i));
		}
	}

	float scrollPosition = inventoryScrollBar_->getScrollPosition();
	float stepSize	     = inventoryScrollBar_->getStepSize();
	float documentSize   = stepSize * std::ceilf(inventoryItems_.size() / stepSize);

	inventoryScrollBar_->setDocumentSize(documentSize);
	inventoryScrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	unsigned int startPosition = static_cast<unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < inventoryItems_.size())
		{
			setItemSlot(*inventorySlots_[i], inventoryItems_[index]);
			if (selectedItem_ == inventorySlots_[i]->getUserData())
			{
				selectedSlot_ = inventorySlots_[i];
				selectedSlot_->addChildWindow(slotSelection_);
			}
		}
		else
		{
			setItemSlot(*inventorySlots_[i], nullptr);
		}
	}

	scrollPosition = shopScrollBar_->getScrollPosition();
	stepSize	   = shopScrollBar_->getStepSize();

	startPosition = static_cast<unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < SHOP_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < shopItems_.size())
		{
			setItemSlot(*shopSlots_[i], &shopItems_[index]);
			if (selectedItem_ == shopSlots_[i]->getUserData())
			{
				selectedSlot_ = shopSlots_[i];
				selectedSlot_->addChildWindow(slotSelection_);
			}
		}
		else
		{
			setItemSlot(*shopSlots_[i], nullptr);
		}
	}
}

bool PosShopWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool PosShopWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	const String& name = mouseEventArgs.window->getName();
	const String::size_type found = name.find("PosShop/HandSlot");
	if (found != String::npos)
	{
		inventoryCanScroll_ = true;
		shopCanScroll_ = false;
	}
	else
	{
		inventoryCanScroll_ = false;
		shopCanScroll_ = true;
	}
	return handleMouseEnterItemSlotArea(nullptr, nullptr, e);
}

bool PosShopWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

bool PosShopWindow::HandleSlotMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		const ItemAttributeComponent* item = static_cast<const ItemAttributeComponent*>(mouseEventArgs.window->getUserData());
		if (item != nullptr && !item->Empty())
		{
			selectedSlot_ = mouseEventArgs.window;
			selectedItem_ = static_cast<const ItemAttributeComponent*>(selectedSlot_->getUserData());
			selectedSlot_->addChildWindow(slotSelection_);

			// Set the sale price text using the selected item.
			cost_->setText(PropertyHelper::uintToString(item->GetMoney()));
		}
		else
		{
			cost_->setText("");
		}
	}
	return true;
}

void PosShopWindow::HandlePosShop(GamePacket& packet)
{
	sf::Uint8 type;
	packet >> type;
	if (type == 2) // Buy result
	{
		processing_ = false;
		return;
	}

	// Pos shop open
	sf::Uint16 itemCount;
	packet >> itemCount;

	ItemArray().swap(shopItems_);
	shopItems_.resize(itemCount);
	for (unsigned int i = 0; i < itemCount; ++i)
	{
		shopItems_[i].HandleItemReceive(packet, GameProtocol::ITEM_SHOP, false);
		if (shopItems_[i].GetDura() == 0)
		{
			shopItems_[i].SetDura(1);
		}
	}

	const float scrollPosition = shopScrollBar_->getScrollPosition();
	const float stepSize	   = shopScrollBar_->getStepSize();
	const float documentSize   = stepSize * std::ceilf(shopItems_.size() / stepSize);

	shopScrollBar_->setDocumentSize(documentSize);
	shopScrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	window_->show();
}

bool PosShopWindow::HandleScrollInventory(const EventArgs& e)
{
	if (!inventoryCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*inventoryScrollBar_, e, "PosShop/HandSlot");
}

bool PosShopWindow::HandleScrollShop(const EventArgs& e)
{
	if (!shopCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*shopScrollBar_, e, "PosShop/StoreSlot");
}

bool PosShopWindow::HandleBuyButtonClicked(const EventArgs& e)
{
	if (selectedSlot_ == nullptr || processing_)
	{
		return true;
	}

	// Can only buy if the selected item is within the shop inventory.
	String name = selectedSlot_->getName();
	String::size_type found = name.find("PosShop/StoreSlot");
	if (found == String::npos)
	{
		return true;
	}

	if (selectedItem_ != nullptr && !selectedItem_->Empty())
	{
		if (selectedItem_->GetQuantity() > 1)
		{
			Game::Instance().inputBoxWindow->Show("How Many?", Event::Subscriber(&PosShopWindow::HandleBuyItemInputResult, this));
			Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(selectedItem_->GetDura()));
		}
		else
		{
			SendShopBuy(selectedItem_->GetId(), 1);
		}
	}
	return true;
}

void PosShopWindow::SendShopBuy(const unsigned short id, const unsigned short howMany)
{
	GamePacket packet(Network::GameProtocol::PKT_CLASSPOINTBUY);
	packet << static_cast<sf::Uint8>(2); // Buy
	packet << static_cast<sf::Uint16>(id);
	packet << static_cast<sf::Uint16>(howMany);
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

bool PosShopWindow::HandleInventoryChanged(const EventArgs& e)
{
	selectedItem_ = nullptr;
	cost_->setText("");
	Update();
	return true;
}

bool PosShopWindow::HandleBuyItemInputResult(const EventArgs& e)
{
	const int howMany = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (howMany > 0 && selectedItem_ != nullptr && !selectedItem_->Empty() &&
		(selectedItem_->GetShopQuantity() >= howMany || selectedItem_->GetShopQuantity() == -1))
	{
		SendShopBuy(selectedItem_->GetId(), howMany);
	}
	return true;
}

} // namespace GUI