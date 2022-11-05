
#include "GUI/ExchangeWindow.h"
#include "GUI/MessageBoxWindow.h"
#include "GUI/InputBoxWindow.h"
#include "GUI/Common/Item.h"

#include "Components/AttributeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/ItemAttributeComponent.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;
using namespace artemis;

ExchangeWindow::ExchangeWindow() :
trading_(false),
otherServerId_(-1),
myOK_(false),
otherOK_(false),
myCanScroll_(false),
otherCanScroll_(false),
inventoryCanScroll_(false),
processing_(false),
processItem_(nullptr),
processItemCount_(0)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("exchangewindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&ExchangeWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&ExchangeWindow::HandleHidden, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&ExchangeWindow::HandleScrollInventory, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&ExchangeWindow::HandleScrollMy, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&ExchangeWindow::HandleScrollOther, this));

		closeButton_ = static_cast<PushButton*>(window_->getChild("Exchange/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ExchangeWindow::HandleClose, this));

		myNameText_ = window_->getChild("Exchange/MyName");

		otherNameText_ = window_->getChild("Exchange/OtherName");

		myOKCheckbox_ = static_cast<Checkbox*>(window_->getChild("Exchange/MyOKCheckbox"));
		myOKCheckbox_->subscribeEvent(Checkbox::EventCheckStateChanged,
			Event::Subscriber(&ExchangeWindow::HandleOKCheckboxStateChanged, this));

		otherOKCheckbox_ = static_cast<Checkbox*>(window_->getChild("Exchange/OtherOKCheckbox"));
		otherOKCheckbox_->setMousePassThroughEnabled(true);

		myScrollbar_ = static_cast<Scrollbar*>(window_->getChild("Exchange/MyScrollbar"));
		myScrollbar_->setStepSize(2.0f);
		myScrollbar_->setPageSize(static_cast<float>(TRADE_SLOT_COUNT));
		myScrollbar_->setOverlapSize(0);
		myScrollbar_->setDocumentSize(0);
		myScrollbar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&ExchangeWindow::HandleScrollPositionChanged, this));

		otherScrollbar_ = static_cast<Scrollbar*>(window_->getChild("Exchange/OtherScrollbar"));
		otherScrollbar_->setStepSize(2.0f);
		otherScrollbar_->setPageSize(static_cast<float>(TRADE_SLOT_COUNT));
		otherScrollbar_->setOverlapSize(0);
		otherScrollbar_->setDocumentSize(0);
		otherScrollbar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&ExchangeWindow::HandleScrollPositionChanged, this));

		inventoryScrollbar_ = static_cast<Scrollbar*>(window_->getChild("Exchange/InventoryScrollbar"));
		inventoryScrollbar_->setStepSize(5.0f);
		inventoryScrollbar_->setPageSize(static_cast<float>(INVENTORY_SLOT_COUNT));
		inventoryScrollbar_->setOverlapSize(0);
		inventoryScrollbar_->setDocumentSize(0);
		inventoryScrollbar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&ExchangeWindow::HandleScrollPositionChanged, this));

		for (int i = 0; i < TRADE_SLOT_COUNT; ++i)
		{
			mySlots_[i] = window_->getChild("Exchange/MySlot" + PropertyHelper::intToString(i+1));
			mySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&ExchangeWindow::HandleMouseEnterSlotArea, this));
			mySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&ExchangeWindow::HandleMouseLeaveSlotArea, this));
			mySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&ExchangeWindow::HandleScrollMy, this));
			mySlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&ExchangeWindow::HandleMySlotDragDropItemDropped, this));

			otherSlots_[i] = window_->getChild("Exchange/OtherSlot" + PropertyHelper::intToString(i+1));
			otherSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&ExchangeWindow::HandleMouseEnterSlotArea, this));
			otherSlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&ExchangeWindow::HandleMouseLeaveSlotArea, this));
			mySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&ExchangeWindow::HandleScrollOther, this));
		}

		for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
		{
			inventorySlots_[i] = window_->getChild("Exchange/InventorySlot" + PropertyHelper::intToString(i+1));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&ExchangeWindow::HandleMouseEnterSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&ExchangeWindow::HandleMouseLeaveSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&ExchangeWindow::HandleScrollInventory, this));
		}

		mySlotArea_ = window_->getChild("Exchange/MySlotArea");
		mySlotArea_->subscribeEvent(Window::EventDragDropItemDropped,
			Event::Subscriber(&ExchangeWindow::HandleMySlotDragDropItemDropped, this));

		myMoney_ = window_->getChild("Exchange/MyMoney");
		myMoney_->setText("");

		otherMoney_ = window_->getChild("Exchange/OtherMoney");
		otherMoney_->setText("");

		inventoryMoney_ = window_->getChild("Exchange/InventoryMoney");

		moneyButton_ = static_cast<PushButton*>(window_->getChild("Exchange/MoneyButton"));
		moneyButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&ExchangeWindow::HandleMoneyButtonClicked, this));

		dragItem_ = createItemDragItem("Exchange/DragItem");

		dragContainer_ = createItemDragContainer("Exchange/DragContainer", dragItem_);
		dragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&ExchangeWindow::HandleDragStarted, this));
		dragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&ExchangeWindow::HandleDragEnded, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "ExchangeWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

ExchangeWindow::~ExchangeWindow()
{
	dragItem_->destroy();
	dragContainer_->destroy();
	window_->destroy();
}

bool ExchangeWindow::HandleShown(const EventArgs& e)
{
	window_->setModalState(true);
	return true;
}

bool ExchangeWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool ExchangeWindow::HandleClose(const EventArgs& e)
{
	SendTradeCancel();
	Reset();
	window_->hide();
	return true;
}

void ExchangeWindow::HandleAskTrade(Entity& target)
{
	if (!trading_)
	{
		//TODO: Use Component Mapper?
		AttributeComponent* targetAttributeComponent = target.getComponent<AttributeComponent>();
		if (targetAttributeComponent != nullptr)
		{
			trading_ = true;
			otherServerId_ = targetAttributeComponent->GetServerId();
			otherName_ = targetAttributeComponent->GetName();

			Game::Instance().messageBoxWindow->Show("Request a trade with " + otherName_ + "?",
				GUI::MessageBoxMode::OKCancel, Event::Subscriber(&ExchangeWindow::HandleAskTradeMessageBoxResult, this));
		}
	}
}

bool ExchangeWindow::HandleAskTradeMessageBoxResult(const EventArgs& e)
{
	if (Game::Instance().messageBoxWindow->GetResult() == MessageBoxResult::OK)
	{
		SendTradeRequest();
		Game::Instance().messageBoxWindow->Show("Waiting for a reply from " + otherName_ + ".",
			GUI::MessageBoxMode::Cancel, Event::Subscriber(&ExchangeWindow::HandleWaitTradeMessageBoxResult, this));
	}
	else
	{
		Reset();
	}
	return true;
}

void ExchangeWindow::SendTradeRequest()
{
	GamePacket packet(GameProtocol::PKT_CLIENTEVENT);
	packet << static_cast<sf::Uint8>(3) << otherServerId_ << otherName_;
	Game::Instance().gameSocket->Send(packet);
}

bool ExchangeWindow::HandleWaitTradeMessageBoxResult(const EventArgs& e)
{
	if (Game::Instance().messageBoxWindow->GetResult() == MessageBoxResult::Cancel)
	{
		SendTradeCancel();
		Reset();
	}
	return true;
}

void ExchangeWindow::SendTradeCancel()
{
	GamePacket packet(GameProtocol::PKT_TRADE_CANCEL);
	packet << otherServerId_;
	Game::Instance().gameSocket->Send(packet);
}

void ExchangeWindow::Reset()
{
	trading_ = false;
	processing_ = false;
	processItem_ = nullptr;
	processItemCount_ = 0;

	otherServerId_ = -1;
	otherName_ = "";

	myOK_ = false;
	myOKCheckbox_->setSelected(false);
	myMoney_->setText("");
	myItems_.clear();
	myScrollbar_->setScrollPosition(0);

	otherOK_ = false;
	otherOKCheckbox_->setSelected(false);
	otherMoney_->setText("");
	otherItems_.clear();
	otherScrollbar_->setScrollPosition(0);

	inventoryScrollbar_->setScrollPosition(0);
}

void ExchangeWindow::HandleTradeRequest(GamePacket& packet)
{
	sf::Int32 serverId;
	packet >> serverId;

	std::string name;
	packet >> name;

	trading_ = true;
	otherServerId_ = serverId;
	otherName_ = name;

	// TODO: If another GUI window is open then the trade should be canceled because the player is busy.
	Game::Instance().messageBoxWindow->Show(otherName_ + " asked for a Transaction.\nDo you wish to trade with him?",
		GUI::MessageBoxMode::OKCancel, Event::Subscriber(&ExchangeWindow::HandleTradeRequestMessageBoxResult, this));
}

bool ExchangeWindow::HandleTradeRequestMessageBoxResult(const EventArgs& e)
{
	if (Game::Instance().messageBoxWindow->GetResult() == MessageBoxResult::OK)
	{
		SendTradeAccept(1);
	}
	else
	{
		SendTradeAccept(2);
		Reset();
	}
	return true;
}

void ExchangeWindow::SendTradeAccept(unsigned char type)
{
	GamePacket packet(GameProtocol::PKT_TRADE_ACK);
	packet << otherServerId_ << otherName_ << type;
	Game::Instance().gameSocket->Send(packet);
}

void ExchangeWindow::Update()
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent =Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent == nullptr)
	{
		return;
	}

	inventoryMoney_->setText(PropertyHelper::uintToString(attributeComponent->GetMoney()));

	myNameText_->setText(attributeComponent->GetName());
	otherNameText_->setText(otherName_);

	float scrollPosition = inventoryScrollbar_->getScrollPosition();
	float stepSize	     = inventoryScrollbar_->getStepSize();
	float documentSize   = stepSize * std::ceilf(inventoryItems_.size() / stepSize);

	inventoryScrollbar_->setDocumentSize(documentSize);
	inventoryScrollbar_->setScrollPosition(std::min(scrollPosition, documentSize));

	unsigned int startPosition = static_cast<const unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < inventoryItems_.size())
		{
			setItemSlot(*inventorySlots_[i], &inventoryItems_[index]);
		}
		else
		{
			setItemSlot(*inventorySlots_[i], nullptr);
		}
	}

	scrollPosition = myScrollbar_->getScrollPosition();
	stepSize	   = myScrollbar_->getStepSize();
	documentSize   = stepSize * std::ceilf(myItems_.size() / stepSize);

	myScrollbar_->setDocumentSize(documentSize);
	myScrollbar_->setScrollPosition(std::min(scrollPosition, documentSize));

	startPosition = static_cast<unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < TRADE_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < myItems_.size())
		{
			setItemSlot(*mySlots_[i], &myItems_[index]);
		}
		else
		{
			setItemSlot(*mySlots_[i], nullptr);
		}
	}

	scrollPosition = otherScrollbar_->getScrollPosition();
	stepSize	   = otherScrollbar_->getStepSize();
	documentSize   = stepSize * std::ceilf(otherItems_.size() / stepSize);

	otherScrollbar_->setDocumentSize(documentSize);
	otherScrollbar_->setScrollPosition(std::min(scrollPosition, documentSize));

	startPosition = static_cast<unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < TRADE_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < otherItems_.size())
		{
			setItemSlot(*otherSlots_[i], &otherItems_[index]);
		}
		else
		{
			setItemSlot(*otherSlots_[i], nullptr);
		}
	}
}

void ExchangeWindow::HandleTradeAccept(GamePacket& packet)
{
	sf::Uint8 result;
	packet >> result;

	Game::Instance().messageBoxWindow->Hide(false);

	if (result == GameProtocol::SUCCESS)
	{
		InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
		if (inventoryComponent != nullptr)
		{
			ItemArray().swap(inventoryItems_);
			for (unsigned int i = EQUIPMENT_SLOT_COUNT; i < inventoryComponent->GetInventoryItemSize(); ++i)
			{
				if (!inventoryComponent->GetInventoryItem(i).Empty())
				{
					inventoryComponent->GetInventoryItem(i).SetIndex(i);
					inventoryItems_.push_back(inventoryComponent->GetInventoryItem(i));
				}
			}
		}

		Update();
		window_->show();
	}
	else
	{
		Reset();
		Game::Instance().messageBoxWindow->Show("The other player refused the transaction.");
	}
}

bool ExchangeWindow::HandleOKCheckboxStateChanged(const EventArgs& e)
{
	if (myOK_)
	{
		myOKCheckbox_->setSelected(true);
		return true;
	}

	if (myOKCheckbox_->isSelected())
	{
		myOK_ = true;
		SendTradeOK();
	}
	return true;
}

void ExchangeWindow::SendTradeOK()
{
	GamePacket packet(GameProtocol::PKT_TRADE_OK);
	Game::Instance().gameSocket->Send(packet);
}

void ExchangeWindow::HandleTradeOK(GamePacket& packet)
{
	otherOK_ = true;
	otherOKCheckbox_->setSelected(true);
}

void ExchangeWindow::HandleTradeResult(GamePacket& packet)
{
	Reset();
	window_->hide();

	sf::Uint8 result;
	packet >> result;

	switch (result)
	{
	case 0:
		Game::Instance().messageBoxWindow->Show("The transaction has been successfully completed.");
		break;
	case 1:
		Game::Instance().messageBoxWindow->Show("The transaction has failed.");
		break;
	case 2:
		Game::Instance().messageBoxWindow->Show("You're carrying to much weight, therefore transaction cannot be performed.");
		break;
	case 3:
		Game::Instance().messageBoxWindow->Show("Since there is too much money involved, transaction cannot be done.");
		break;
	case 4:
		Game::Instance().messageBoxWindow->Show("You're too far from the other player to complete the transaction.");
		break;
	}
}

void ExchangeWindow::HandleTradeCancel(GamePacket& packet)
{
	Reset();
	window_->hide();
	Game::Instance().messageBoxWindow->Show("The other player cancelled the transaction.");
}

bool ExchangeWindow::HandleMoneyButtonClicked(const EventArgs& e)
{
	Game::Instance().inputBoxWindow->Show("How Much?", Event::Subscriber(&ExchangeWindow::HandleMoneyInputResult, this));
	return true;
}

bool ExchangeWindow::HandleMoneyInputResult(const EventArgs& e)
{
	if (myOK_ || otherOK_)
	{
		return true;
	}

	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent == nullptr)
	{
		return true;
	}

	const unsigned int money = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (money > 0 && money <= attributeComponent->GetMoney())
	{
		SendTradeMoney(money);
	}
	return true;
}

void ExchangeWindow::SendTradeMoney(const unsigned int money)
{
	GamePacket packet(GameProtocol::PKT_TRADE_SETMONEY);
	packet << money;
	Game::Instance().gameSocket->Send(packet);
}

void ExchangeWindow::HandleTradeMoney(GamePacket& packet)
{
	sf::Int32 serverId;
	packet >> serverId;

	sf::Uint32 money;
	packet >> money;

	if (serverId == otherServerId_)
	{
		otherMoney_->setText(PropertyHelper::uintToString(money));
	}
	else
	{
		myMoney_->setText(PropertyHelper::uintToString(money));
	}
}

bool ExchangeWindow::HandleDragStarted(const EventArgs& e)
{
	if (processing_)
	{
		dragContainer_->setDraggingEnabled(false);
		return true;
	}
	return handleItemDragStarted(*dragContainer_, *dragItem_, e);
}

bool ExchangeWindow::HandleDragEnded(const EventArgs& e)
{
	return handleItemDragEnded(*dragItem_, e);
}

bool ExchangeWindow::HandleScrollMy(const EventArgs& e)
{
	if (!myCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*myScrollbar_, e, "Exchange/MySlot");
}

bool ExchangeWindow::HandleScrollOther(const EventArgs& e)
{
	if (!otherCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*otherScrollbar_, e, "Exchange/OtherSlot");
}

bool ExchangeWindow::HandleScrollInventory(const EventArgs& e)
{
	if (!inventoryCanScroll_)
	{
		return true;
	}
	return handleItemScroll(*inventoryScrollbar_, e, "Exchange/InventorySlot");
}

bool ExchangeWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool ExchangeWindow::IsSlot(const Window& window, const String& name) const
{
	return window.getName().find(name) != String::npos;
}

bool ExchangeWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);

	if (IsSlot(*mouseEventArgs.window, "Exchange/MySlot"))
	{
		myCanScroll_ = true;
		otherCanScroll_ = false;
		inventoryCanScroll_ = false;
	
	}
	else if (IsSlot(*mouseEventArgs.window, "Exchange/OtherSlot"))
	{
		myCanScroll_ = false;
		otherCanScroll_ = true;
		inventoryCanScroll_ = false;
	}
	else if (IsSlot(*mouseEventArgs.window, "Exchange/InventorySlot"))
	{
		myCanScroll_ = false;
		otherCanScroll_ = false;
		inventoryCanScroll_ = true;
	}

	return handleMouseEnterItemSlotArea(dragContainer_, inventoryCanScroll_ ? dragItem_ : nullptr, e);
}

bool ExchangeWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

void ExchangeWindow::HandleTradeItem(GamePacket& packet)
{
	sf::Uint8 type;
	packet >> type;

	if (type != 3)
	{
		processing_ = false;
	}

	if (type == 1)
	{
		auto it = std::find_if(myItems_.begin(), myItems_.end(),
			[=](ItemAttributeComponent& item) { return item.GetIndex() == processItem_->GetIndex(); });

		if (it != myItems_.end())
		{
			(*it).SetDura((*it).GetDura() + processItemCount_);
		}
		else
		{
			ItemAttributeComponent item = *processItem_;
			item.SetDura(processItemCount_);
			myItems_.push_back(item);
		}

		if (processItemCount_ != processItem_->GetDura() &&
			processItem_->GetType() >= ItemType::Potion && processItem_->GetType() <= ItemType::Quest)
		{
			processItem_->SetDura(processItem_->GetDura() - processItemCount_);
		}
		else
		{
			inventoryItems_.erase(
				std::remove_if(inventoryItems_.begin(), inventoryItems_.end(),
				[=] (ItemAttributeComponent& item) { return item.GetIndex() == processItem_->GetIndex(); }),
				inventoryItems_.end());
		}

		Update();
	}
	else if (type == 3)
	{
		sf::Uint16 itemIndex;
		packet >> itemIndex;

		bool addItem = true;

		ItemAttributeComponent otherItem;
		otherItem.SetIndex(itemIndex);
		otherItem.HandleItemReceive(packet, GameProtocol::ITEM_EXCHANGE, false);

		for (auto it = otherItems_.begin(); it != otherItems_.end(); ++it)
		{
			if (it->GetIndex() == itemIndex)
			{
				(*it).SetDura((*it).GetDura() + otherItem.GetDura());
				addItem = false;
			}
		}

		if (addItem)
		{
			otherItems_.push_back(otherItem);

			const float stepSize = otherScrollbar_->getStepSize();
			const float documentSize = stepSize * std::ceilf(otherItems_.size() / stepSize);
			otherScrollbar_->setDocumentSize(documentSize);
		}

		Update();
	}
}

bool ExchangeWindow::HandleMySlotDragDropItemDropped(const EventArgs& e)
{
	const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem->getName().compare("Exchange/DragContainer") == 0)
	{
		ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(dragDropArgs.dragDropItem->getUserData());
		if (item != nullptr && !item->Empty())
		{
			if (item->GetQuantity() > 1)
			{
				Game::Instance().inputBoxWindow->Show("How Many?",
					Event::Subscriber(&ExchangeWindow::HandleAddItemInputResult, this));
				Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(item->GetQuantity()));
				Game::Instance().inputBoxWindow->SetUserData(item);
			}
			else
			{
				SendTradeAddItem(item->GetIndex(), 1);
			}
		}
	}
	return true;
}

bool ExchangeWindow::HandleAddItemInputResult(const EventArgs& e)
{
	const unsigned int howMany = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (howMany > 0)
	{
		const ItemAttributeComponent* item = static_cast<const ItemAttributeComponent*>(Game::Instance().inputBoxWindow->GetUserData());
		if (item != nullptr && !item->Empty() && item->GetQuantity() >= howMany)
		{
			SendTradeAddItem(item->GetIndex(), howMany);
		}
	}
	return true;
}

void ExchangeWindow::SendTradeAddItem(const unsigned short index, const unsigned short count)
{
	processing_ = true;
	processItem_ = static_cast<ItemAttributeComponent*>(dragContainer_->getUserData());
	processItemCount_ = count;

	GamePacket packet(GameProtocol::PKT_TRADE_ADDITEM);
	packet << index << count;
	Game::Instance().gameSocket->Send(packet);
}


} // namespace GUI