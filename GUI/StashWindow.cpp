
#include "GUI/StashWindow.h"
#include "GUI/InputBoxWindow.h"
#include "GUI/Common/Item.h"

#include "Network/GameProtocol.h"
#include "Network/GamePacket.h"
#include "Network/GameSocket.h"

#include "Components/AttributeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/ItemAttributeComponent.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

String StashWindow::EventInventoryChanged = "EventInventoryChanged";

StashWindow::StashWindow() :
selectedSlot_(nullptr),
selectedItem_(nullptr),
inventoryCanScroll_(false),
storageCanScroll_(false),
processing_(false),
itemCountMax_(0)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("stashwindow.layout");
		window_->hide();
		window_->addEvent(EventInventoryChanged);
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&StashWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&StashWindow::HandleHidden, this));
		window_->subscribeEvent(EventInventoryChanged, Event::Subscriber(&StashWindow::HandleInventoryChanged, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&StashWindow::HandleScrollInventory, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&StashWindow::HandleScrollStorage, this));

		slotSelection_ = window_->getChild("Stash/SlotSelection");
		window_->removeChildWindow(slotSelection_);

		closeButton_ = static_cast<PushButton*>(window_->getChild("Stash/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&StashWindow::HandleClose, this));

		putItemButton_ = static_cast<PushButton*>(window_->getChild("Stash/PutItemButton"));
		putItemButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&StashWindow::HandlePutItemButtonClicked, this));

		getItemButton_ = static_cast<PushButton*>(window_->getChild("Stash/GetItemButton"));
		getItemButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&StashWindow::HandleGetItemButtonClicked, this));

		putMoneyButton_ = static_cast<PushButton*>(window_->getChild("Stash/PutMoneyButton"));
		putMoneyButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&StashWindow::HandlePutMoneyButtonClicked, this));

		getMoneyButton_ = static_cast<PushButton*>(window_->getChild("Stash/GetMoneyButton"));
		getMoneyButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&StashWindow::HandleGetMoneyButtonClicked, this));

		inventoryScrollBar_ = static_cast<Scrollbar*>(window_->getChild("Stash/InventoryScrollbar"));
		inventoryScrollBar_->setStepSize(2.0f);
		inventoryScrollBar_->setPageSize(static_cast<float>(INVENTORY_SLOT_COUNT));
		inventoryScrollBar_->setOverlapSize(0);
		inventoryScrollBar_->setDocumentSize(0);
		inventoryScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&StashWindow::HandleScrollPositionChanged, this));

		storageScrollBar_ = static_cast<Scrollbar*>(window_->getChild("Stash/StorageScrollbar"));
		storageScrollBar_->setStepSize(2.0f);
		storageScrollBar_->setPageSize(static_cast<float>(STORAGE_SLOT_COUNT));
		storageScrollBar_->setOverlapSize(0);
		storageScrollBar_->setDocumentSize(0);
		storageScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&StashWindow::HandleScrollPositionChanged, this));

		inventoryMoney_ = window_->getChild("Stash/InventoryMoney");

		storageMoney_ = window_->getChild("Stash/StorageMoney");

		for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
		{
			inventorySlots_[i] = window_->getChild("Stash/InventorySlot" + PropertyHelper::intToString(i+1));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&StashWindow::HandleMouseEnterSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&StashWindow::HandleMouseLeaveSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&StashWindow::HandleSlotMouseClick, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&StashWindow::HandleScrollInventory, this));
		}

		for (int i = 0; i < STORAGE_SLOT_COUNT; ++i)
		{
			storageSlots_[i] = window_->getChild("Stash/StorageSlot" + PropertyHelper::intToString(i+1));
			storageSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&StashWindow::HandleMouseEnterSlotArea, this));
			storageSlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&StashWindow::HandleMouseLeaveSlotArea, this));
			storageSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&StashWindow::HandleSlotMouseClick, this));
			storageSlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&StashWindow::HandleScrollStorage, this));
		}

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "StashWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

StashWindow::~StashWindow()
{
	slotSelection_->destroy();
	window_->destroy();
}

bool StashWindow::HandleShown(const EventArgs& e)
{
	selectedItem_ = nullptr;
	storageScrollBar_->setScrollPosition(0);
	window_->moveToFront();
	window_->setModalState(true);
	Update();
	return true;
}

bool StashWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool StashWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

void StashWindow::Update()
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (attributeComponent == nullptr || inventoryComponent == nullptr)
	{
		return;
	}

	inventoryMoney_->setText(PropertyHelper::uintToString(attributeComponent->GetMoney()));

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
	float stepSize		 = inventoryScrollBar_->getStepSize();
	float documentSize	 = stepSize * std::ceilf(inventoryItems_.size() / stepSize);

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

	scrollPosition = storageScrollBar_->getScrollPosition();
	stepSize	   = storageScrollBar_->getStepSize();

	startPosition = static_cast<unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < STORAGE_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < storageItems_.size())
		{
			setItemSlot(*storageSlots_[i], &storageItems_[index]);
			if (selectedItem_ == storageSlots_[i]->getUserData())
			{
				selectedSlot_ = storageSlots_[i];
				selectedSlot_->addChildWindow(slotSelection_);
			}
		}
		else
		{
			setItemSlot(*storageSlots_[i], nullptr);
		}
	}
}

bool StashWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool StashWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	const String& name = mouseEventArgs.window->getName();
	const String::size_type found = name.find("Stash/InventorySlot");
	if (found != String::npos)
	{
		inventoryCanScroll_ = true;
		storageCanScroll_ = false;
	}
	else
	{
		inventoryCanScroll_ = false;
		storageCanScroll_ = true;
	}
	return handleMouseEnterItemSlotArea(nullptr, nullptr, e);
}

bool StashWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

bool StashWindow::HandleSlotMouseClick(const EventArgs& e)
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
		}
	}
	return true;
}

void StashWindow::HandleOpenStorage(GamePacket& packet)
{
	sf::Uint32 storageMoney;
	packet >> storageMoney;
	storageMoney_->setText(PropertyHelper::uintToString(storageMoney));

	sf::Uint16 itemCountMax;
	packet >> itemCountMax;
	itemCountMax_ = itemCountMax;

	sf::Uint16 itemCount;
	packet >> itemCount;

	ItemArray().swap(storageItems_);
	storageItems_.resize(itemCount);
	for (unsigned int i = 0; i < itemCount; ++i)
	{
		storageItems_[i].HandleItemReceive(packet, GameProtocol::ITEM_STORAGE);
	}

	std::sort(storageItems_.begin(), storageItems_.end());

	const float scrollPosition = storageScrollBar_->getScrollPosition();
	const float stepSize	   = storageScrollBar_->getStepSize();
	const float documentSize   = stepSize * std::ceilf(itemCountMax_ / stepSize);

	storageScrollBar_->setDocumentSize(documentSize);
	storageScrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	window_->show();
}

bool StashWindow::HandleScrollInventory(const EventArgs& e)
{
	if (!inventoryCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*inventoryScrollBar_, e, "Stash/InventorySlot");
}

bool StashWindow::HandleScrollStorage(const EventArgs& e)
{
	if (!storageCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*storageScrollBar_, e, "Stash/StorageSlot");
}

bool StashWindow::HandlePutItemButtonClicked(const EventArgs& e)
{
	if (!selectedItem_ || processing_)
	{
		return true;
	}

	// Can only put item in storage if the selected item is within the characters inventory.
	const String& name = selectedSlot_->getName();
	const String::size_type found = name.find("Stash/InventorySlot");
	if (found == String::npos)
	{
		return true;
	}

	if (!selectedItem_->Empty())
	{
		if (selectedItem_->GetQuantity() > 1)
		{
			Game::Instance().inputBoxWindow->Show("How Many?", Event::Subscriber(&StashWindow::HandlePutItemInputResult, this));
			Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(selectedItem_->GetQuantity()));
		}
		else
		{
			SendPutItem(selectedItem_->GetIndex(), 1);
		}
	}
	return true;
}

bool StashWindow::HandleGetItemButtonClicked(const EventArgs& e)
{
	if (!selectedItem_ || processing_)
	{
		return true;
	}

	// Can only get item from storage if the selected item is within the storage inventory.
	const String& name = selectedSlot_->getName();
	const String::size_type found = name.find("Stash/StorageSlot");
	if (found == String::npos)
	{
		return true;
	}

	if (!selectedItem_->Empty())
	{
		if (selectedItem_->GetQuantity() > 1)
		{
			Game::Instance().inputBoxWindow->Show("How Many?", Event::Subscriber(&StashWindow::HandleGetItemInputResult, this));
			Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(selectedItem_->GetQuantity()));
		}
		else
		{
			SendGetItem(selectedItem_->GetIndex(), 1);
		}
	}
	return true;
}

bool StashWindow::HandlePutMoneyButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		Game::Instance().inputBoxWindow->Show("How Much?", Event::Subscriber(&StashWindow::HandlePutMoneyInputResult, this));
	}
	return true;
}

bool StashWindow::HandleGetMoneyButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		Game::Instance().inputBoxWindow->Show("How Much?", Event::Subscriber(&StashWindow::HandleGetMoneyInputResult, this));
	}
	return true;
}

bool StashWindow::HandleInventoryChanged(const EventArgs& e)
{
	selectedItem_ = nullptr;
	Update();
	return true;
}

void StashWindow::SendPutItem(const unsigned short index, const unsigned short howMany)
{
	GamePacket packet(GameProtocol::PKT_SAVEITEM);
	packet << static_cast<sf::Uint16>(index);
	packet << static_cast<sf::Uint16>(howMany);
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

void StashWindow::SendGetItem(const unsigned short index, const unsigned short howMany)
{
	GamePacket packet(GameProtocol::PKT_TAKEBACKITEM);
	packet << static_cast<sf::Uint16>(index);
	packet << static_cast<sf::Uint16>(howMany);
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

void StashWindow::SendPutMoney(const unsigned int money)
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent != nullptr && attributeComponent->GetMoney() >= money)
	{
		GamePacket packet(GameProtocol::PKT_SAVEMONEY);
		packet << static_cast<sf::Uint32>(money);
		Game::Instance().gameSocket->Send(packet);
		processing_ = true;
	}
}

void StashWindow::SendGetMoney(const unsigned int money)
{
	const unsigned int storageMoney = PropertyHelper::stringToUint(storageMoney_->getText());
	if (storageMoney >= money)
	{
		GamePacket packet(GameProtocol::PKT_TAKEBACKMONEY);
		packet << static_cast<sf::Uint32>(money);
		Game::Instance().gameSocket->Send(packet);
		processing_ = true;
	}
}

void StashWindow::HandlePutItem(GamePacket& packet)
{
	processing_ = false;

	sf::Int16 inventoryItemIndex;
	packet >> inventoryItemIndex;
	if (inventoryItemIndex == -1)
	{
		return;
	}

	sf::Uint16 remainingDura;
	packet >> remainingDura;

	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (inventoryComponent != nullptr)
	{
		inventoryComponent->GetInventoryItem(inventoryItemIndex).SetDura(remainingDura);
	}

	sf::Uint16 storageItemIndex;
	packet >> storageItemIndex;

	ItemAttributeComponent storageItem;
	storageItem.SetIndex(storageItemIndex);
	storageItem.HandleItemReceive(packet, GameProtocol::ITEM_STORAGE, false);

	auto it = std::find_if(storageItems_.begin(), storageItems_.end(),
		[=](ItemAttributeComponent& item) { return item.GetIndex() == storageItemIndex; });
	if (it != storageItems_.end())
	{
		(*it) = storageItem;
	}
	else
	{
		storageItems_.push_back(storageItem);

		std::sort(storageItems_.begin(), storageItems_.end());

		int stepSize = static_cast<int>(storageScrollBar_->getStepSize());
		int documentSize = (stepSize * ((itemCountMax_ / stepSize) + ((itemCountMax_ % stepSize) ? 1 : 0)));
		documentSize = std::max(0, documentSize);
		storageScrollBar_->setDocumentSize(static_cast<float>(documentSize));
	}

	selectedItem_ = nullptr;
	Update();
}

void StashWindow::HandleGetItem(GamePacket& packet)
{
	processing_ = false;

	sf::Int16 storageItemIndex;
	packet >> storageItemIndex;
	if (storageItemIndex == -1)
	{
		return;
	}

	sf::Uint16 remainingDura;
	packet >> remainingDura;
	if (remainingDura == 0)
	{
		storageItems_.erase(
			std::remove_if(storageItems_.begin(), storageItems_.end(),
			[=] (ItemAttributeComponent& item) { return item.GetIndex() == storageItemIndex; }),
			storageItems_.end());

		std::sort(storageItems_.begin(), storageItems_.end());

		int stepSize = static_cast<int>(storageScrollBar_->getStepSize());
		int documentSize = (stepSize * ((itemCountMax_ / stepSize) + ((itemCountMax_ % stepSize) ? 1 : 0)));
		documentSize = std::max(0, documentSize);
		storageScrollBar_->setDocumentSize(static_cast<float>(documentSize));
	}
	else
	{
		auto it = std::find_if(storageItems_.begin(), storageItems_.end(),
			[=](ItemAttributeComponent& item) { return item.GetIndex() == storageItemIndex; });
		if (it != storageItems_.end())
		{
			it->SetDura(remainingDura);
		}
	}

	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (inventoryComponent != nullptr)
	{
		sf::Uint16 inventoryItemIndex;
		packet >> inventoryItemIndex;
		inventoryComponent->GetInventoryItem(inventoryItemIndex).HandleItemReceive(packet, GameProtocol::ITEM_ME, false);
	}

	selectedItem_ = nullptr;
	Update();
}

void StashWindow::HandlePutGetMoney(GamePacket& packet)
{
	processing_ = false;

	sf::Uint32 inventoryMoney;
	packet >> inventoryMoney;

	sf::Uint32 storageMoney;
	packet >> storageMoney;

	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent != nullptr)
	{
		attributeComponent->SetMoney(inventoryMoney);
		inventoryMoney_->setText(PropertyHelper::uintToString(attributeComponent->GetMoney()));
	}
	storageMoney_->setText(PropertyHelper::uintToString(storageMoney));
}

bool StashWindow::HandlePutMoneyInputResult(const EventArgs& e)
{
	const unsigned int money = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (money > 0)
	{
		SendPutMoney(money);
	}
	return true;
}

bool StashWindow::HandleGetMoneyInputResult(const EventArgs& e)
{
	const unsigned int money = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (money > 0)
	{
		SendGetMoney(money);
	}
	return true;
}

bool StashWindow::HandlePutItemInputResult(const EventArgs& e)
{
	const unsigned int howMany = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (howMany > 0 && selectedItem_ != nullptr && selectedItem_->GetQuantity() >= howMany)
	{
		SendPutItem(selectedItem_->GetIndex(), howMany);
	}
	return true;
}

bool StashWindow::HandleGetItemInputResult(const EventArgs& e)
{
	const unsigned int howMany = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (howMany > 0 && selectedItem_ != nullptr && selectedItem_->GetQuantity() >= howMany)
	{
		SendGetItem(selectedItem_->GetIndex(), howMany);
	}
	return true;
}

} // namespace GUI