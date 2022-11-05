
#include "GUI/ShopWindow.h"
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

String ShopWindow::EventShopBuy			 = "EventShopBuy";
String ShopWindow::EventShopSell		 = "EventShopSell";
String ShopWindow::EventInventoryChanged = "EventInventoryChanged";

ShopWindow::ShopWindow() :
shopType_(ShopType::Weapon),
selectedSlot_(nullptr),
selectedItem_(nullptr),
shopNumber_(0),
buyRate_(100),
sellRate_(100),
processing_(false),
inventoryCanScroll_(false),
shopCanScroll_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("shopwindow.layout");
		window_->hide();
		window_->addEvent(EventShopBuy);
		window_->addEvent(EventShopSell);
		window_->addEvent(EventInventoryChanged);
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&ShopWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&ShopWindow::HandleHidden, this));
		window_->subscribeEvent(EventShopBuy, Event::Subscriber(&ShopWindow::HandleShopBuy, this));
		window_->subscribeEvent(EventShopSell, Event::Subscriber(&ShopWindow::HandleShopSell, this));
		window_->subscribeEvent(EventInventoryChanged, Event::Subscriber(&ShopWindow::HandleInventoryChanged, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&ShopWindow::HandleScrollInventory, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&ShopWindow::HandleScrollShop, this));

		slotSelection_ = window_->getChild("Shop/SlotSelection");
		window_->removeChildWindow(slotSelection_);

		shopTypeImage_ = window_->getChild("Shop/Type");

		closeButton_ = static_cast<PushButton*>(window_->getChild("Shop/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ShopWindow::HandleClose, this));

		sellButton_ = static_cast<PushButton*>(window_->getChild("Shop/SellButton"));
		sellButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ShopWindow::HandleSellButtonClicked, this));

		buyButton_ = static_cast<PushButton*>(window_->getChild("Shop/BuyButton"));
		buyButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ShopWindow::HandleBuyButtonClicked, this));

		inventoryScrollBar_ = static_cast<Scrollbar*>(window_->getChild("Shop/HandScrollbar"));
		inventoryScrollBar_->setStepSize(2.0f);
		inventoryScrollBar_->setPageSize(static_cast<float>(INVENTORY_SLOT_COUNT));
		inventoryScrollBar_->setOverlapSize(0);
		inventoryScrollBar_->setDocumentSize(0);
		inventoryScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&ShopWindow::HandleScrollPositionChanged, this));

		shopScrollBar_ = static_cast<Scrollbar*>(window_->getChild("Shop/StoreScrollbar"));
		shopScrollBar_->setStepSize(2.0f);
		shopScrollBar_->setPageSize(static_cast<float>(SHOP_SLOT_COUNT));
		shopScrollBar_->setOverlapSize(0);
		shopScrollBar_->setDocumentSize(0);
		shopScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&ShopWindow::HandleScrollPositionChanged, this));

		money_ = window_->getChild("Shop/Money");

		cost_ = window_->getChild("Shop/Cost");

		for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
		{
			inventorySlots_[i] = window_->getChild("Shop/HandSlot" + PropertyHelper::intToString(i+1));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&ShopWindow::HandleMouseEnterSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&ShopWindow::HandleMouseLeaveSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&ShopWindow::HandleSlotMouseClick, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&ShopWindow::HandleScrollInventory, this));
		}

		for (int i = 0; i < SHOP_SLOT_COUNT; ++i)
		{
			shopSlots_[i] = window_->getChild("Shop/StoreSlot" + PropertyHelper::intToString(i+1));
			shopSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&ShopWindow::HandleMouseEnterSlotArea, this));
			shopSlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&ShopWindow::HandleMouseLeaveSlotArea, this));
			shopSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&ShopWindow::HandleSlotMouseClick, this));
			shopSlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&ShopWindow::HandleScrollShop, this));
		}

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "ShopWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

ShopWindow::~ShopWindow()
{
	slotSelection_->destroy();
	window_->destroy();
}

bool ShopWindow::HandleShown(const EventArgs& e)
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

bool ShopWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool ShopWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

void ShopWindow::Update()
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (attributeComponent == nullptr || inventoryComponent == nullptr)
	{
		return;
	}

	money_->setText(PropertyHelper::uintToString(attributeComponent->GetMoney()));

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

bool ShopWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool ShopWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	const String& name = mouseEventArgs.window->getName();
	const String::size_type found = name.find("Shop/HandSlot");
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

bool ShopWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

bool ShopWindow::HandleSlotMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(mouseEventArgs.window->getUserData());
		if (item != nullptr && !item->Empty())
		{
			selectedSlot_ = mouseEventArgs.window;
			selectedItem_ = static_cast<const ItemAttributeComponent*>(selectedSlot_->getUserData());
			selectedSlot_->addChildWindow(slotSelection_);

			// Set the sale price text using the selected item.
			unsigned int money = selectedItem_->GetMoney();
			const String& name = selectedSlot_->getName();
			const String::size_type found = name.find("Shop/HandSlot");
			if (found != String::npos)
			{
				float temp = selectedItem_->GetMoney() * 0.5f;
				temp *= sellRate_ / 100.f;
				if (selectedItem_->GetType() <= ItemType::Accessory)
				{
					temp *=  static_cast<float>(selectedItem_->GetDura()) / selectedItem_->GetOriginalMaximumDura();
				}
				money = static_cast<unsigned int>(temp);
			}
			cost_->setText(PropertyHelper::uintToString(money));
		}
		else
		{
			cost_->setText("");
		}
	}
	return true;
}

void ShopWindow::SetType(const unsigned int shopType)
{
	switch (shopType)
	{
	case 6:
		shopType_ = ShopType::Weapon;
		break;
	case 7:
		shopType_ = ShopType::Armor;
		break;
	case 8:
		shopType_ = ShopType::Accessory;
		break;
	case 9:
		shopType_ = ShopType::Potion;
		break;
	case 10:
		shopType_ = ShopType::Food;
		break;
	case 11:
		shopType_ = ShopType::Lottery;
		break;
	case 12:
		shopType_ = ShopType::General;
		break;
	default:
		std::cout << "ShopWindow: Invalid type for shop, type=" << shopType_ << std::endl;  
		break;
	}
}

void ShopWindow::HandleOpenShop(GamePacket& packet)
{
	sf::Uint16 shopNumber;
	packet >> shopNumber;
	shopNumber_ = shopNumber;

	sf::Uint16 shopType;
	packet >> shopType;
	SetType(shopType);

	sf::Uint16 buyRate;
	packet >> buyRate;
	buyRate_ = buyRate;

	sf::Uint16 sellRate;
	packet >> sellRate;
	sellRate_ = sellRate;

	sf::Uint16 unknown; // Tax rate, for when a guild owns the guild town.
	packet >> unknown;

	sf::Uint16 itemCount;
	packet >> itemCount;

	ItemArray().swap(shopItems_);
	shopItems_.resize(itemCount);
	for (unsigned int i = 0; i < itemCount; ++i)
	{
		shopItems_[i].HandleItemReceive(packet, GameProtocol::ITEM_SHOP, false);
		shopItems_[i].SetMoney(static_cast<unsigned int>(shopItems_[i].GetMoney() * (buyRate / 100.f)));
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

	switch (shopType_)
	{
	case ShopType::Weapon:
		shopTypeImage_->setProperty("Image", "");
		break;
	case ShopType::Armor:
		shopTypeImage_->setProperty("Image", "set:shopwindow image:Armor");
		break;
	case ShopType::Accessory:
		shopTypeImage_->setProperty("Image", "set:shopwindow image:Accessory");
		break;
	case ShopType::Potion:
		shopTypeImage_->setProperty("Image", "set:shopwindow image:Potion");
		break;
	case ShopType::Food:
		shopTypeImage_->setProperty("Image", "set:shopwindow image:Food");
		break;
	case ShopType::Lottery:
		shopTypeImage_->setProperty("Image", "set:shopwindow image:Lottery");
		break;
	case ShopType::General:
		shopTypeImage_->setProperty("Image", "set:shopwindow image:Store");
		break;
	}

	window_->show();
}

bool ShopWindow::HandleScrollInventory(const EventArgs& e)
{
	if (!inventoryCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*inventoryScrollBar_, e, "Shop/HandSlot");
}

bool ShopWindow::HandleScrollShop(const EventArgs& e)
{
	if (!shopCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*shopScrollBar_, e, "Shop/StoreSlot");
}

bool ShopWindow::HandleSellButtonClicked(const EventArgs& e)
{
	if (selectedSlot_ == nullptr || processing_)
	{
		return true;
	}

	// Can only sell if the selected item is within the characters inventory.
	String name = selectedSlot_->getName();
	String::size_type found = name.find("Shop/HandSlot");
	if (found == String::npos)
	{
		return true;
	}

	if (selectedItem_ != nullptr && !selectedItem_->Empty())
	{
		if (selectedItem_->GetQuantity() > 1)
		{
			Game::Instance().inputBoxWindow->Show("How Many?", Event::Subscriber(&ShopWindow::HandleSellItemInputResult, this));
			Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(selectedItem_->GetQuantity()));
		}
		else
		{
			SendShopSell(selectedItem_->GetIndex(), 1);
		}
	}
	return true;
}

bool ShopWindow::HandleBuyButtonClicked(const EventArgs& e)
{
	if (selectedSlot_ == nullptr || processing_)
	{
		return true;
	}

	// Can only buy if the selected item is within the shop inventory.
	String name = selectedSlot_->getName();
	String::size_type found = name.find("Shop/StoreSlot");
	if (found == String::npos)
	{
		return true;
	}

	if (selectedItem_ != nullptr && !selectedItem_->Empty())
	{
		if ((selectedItem_->GetShopQuantity() > 1 || selectedItem_->GetShopQuantity() == -1) &&
			selectedItem_->GetType() >= ItemType::Potion && selectedItem_->GetType() <= ItemType::Quest)
		{
			Game::Instance().inputBoxWindow->Show("How Many?", Event::Subscriber(&ShopWindow::HandleBuyItemInputResult, this));
			Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(selectedItem_->GetDura()));
		}
		else
		{
			SendShopBuy(selectedItem_->GetId(), 1);
		}
	}
	return true;
}

void ShopWindow::SendShopSell(const unsigned short index, const unsigned short howMany)
{
	GamePacket packet(GameProtocol::PKT_SELL_SHOP);
	packet << static_cast<sf::Uint16>(shopNumber_)
	       << static_cast<sf::Uint16>(index)
	       << static_cast<sf::Uint16>(howMany);
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

void ShopWindow::SendShopBuy(const unsigned short id, const unsigned short howMany)
{
	GamePacket packet(GameProtocol::PKT_BUY_SHOP);
	packet << static_cast<sf::Uint16>(shopNumber_);
	packet << static_cast<sf::Uint16>(id);
	packet << static_cast<sf::Uint16>(howMany);
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

bool ShopWindow::HandleShopSell(const EventArgs& e)
{
	processing_ = false;
	return true;
}

bool ShopWindow::HandleShopBuy(const EventArgs& e)
{
	processing_ = false;
	return true;
}

bool ShopWindow::HandleInventoryChanged(const EventArgs& e)
{
	selectedItem_ = nullptr;
	cost_->setText("");
	Update();
	return true;
}

void ShopWindow::HandleShopItemQuantity(GamePacket& packet)
{
	sf::Uint16 shopNumber;
	packet >> shopNumber;
	if (shopNumber != shopNumber_)
	{
		return;
	}

	sf::Uint16 shopItemIndex;
	packet >> shopItemIndex;
	if (shopItemIndex >= shopItems_.size())
	{
		return;
	}

	ItemAttributeComponent& item = shopItems_[shopItemIndex];
	sf::Uint16 itemId;
	packet >> itemId;
	if (itemId == item.GetId())
	{
		sf::Uint16 itemQuantity;
		packet >> itemQuantity;
		item.SetShopQuantity(itemQuantity);
		Update(); // TODO: Could do with more fine grained updates, so that can update single items or just quantity of item etc.
	}
}

bool ShopWindow::HandleSellItemInputResult(const EventArgs& e)
{
	const unsigned int howMany = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (howMany > 0 && selectedItem_ != nullptr && selectedItem_->GetQuantity() >= howMany)
	{
		SendShopSell(selectedItem_->GetIndex(), howMany);
	}
	return true;
}

bool ShopWindow::HandleBuyItemInputResult(const EventArgs& e)
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