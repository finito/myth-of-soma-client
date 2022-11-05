
#include "GUI/RepairWindow.h"
#include "GUI/MessageBoxWindow.h"
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

String RepairWindow::EventInventoryChanged = "EventInventoryChanged";

RepairWindow::RepairWindow() :
processing_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("repairwindow.layout");
		window_->hide();
		window_->addEvent(EventInventoryChanged);
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&RepairWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&RepairWindow::HandleHidden, this));
		window_->subscribeEvent(EventInventoryChanged, Event::Subscriber(&RepairWindow::HandleInventoryChanged, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&RepairWindow::HandleScrollInventory, this));

		closeButton_ = static_cast<PushButton*>(window_->getChild("Repair/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&RepairWindow::HandleClose, this));

		repairButton_ = static_cast<PushButton*>(window_->getChild("Repair/RepairButton"));
		repairButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&RepairWindow::HandleRepairButtonClicked, this));

		inventoryScrollBar_ = static_cast<Scrollbar*>(window_->getChild("Repair/InventoryScrollbar"));
		inventoryScrollBar_->setStepSize(5.0f);
		inventoryScrollBar_->setPageSize(static_cast<float>(INVENTORY_SLOT_COUNT));
		inventoryScrollBar_->setOverlapSize(0);
		inventoryScrollBar_->setDocumentSize(0);
		inventoryScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&RepairWindow::HandleScrollPositionChanged, this));

		money_ = window_->getChild("Repair/Money");

		for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
		{
			inventorySlots_[i] = window_->getChild("Repair/InventorySlot" + PropertyHelper::intToString(i+1));
			inventorySlots_[i]->setID(i);
			inventorySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&RepairWindow::HandleMouseEnterSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&RepairWindow::HandleMouseLeaveSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&RepairWindow::HandleSlotMouseClickSingle,this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseDoubleClick,
				Event::Subscriber(&RepairWindow::HandleSlotMouseDoubleClick, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&RepairWindow::HandleScrollInventory, this));
			inventorySlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&RepairWindow::HandleInventoryDragDropItemDropped, this));
		}

		repairSlot_ = window_->getChild("Repair/RepairSlot");
		repairSlot_->subscribeEvent(Window::EventMouseEntersArea,
			Event::Subscriber(&RepairWindow::HandleMouseEnterSlotArea, this));
		repairSlot_->subscribeEvent(Window::EventMouseLeavesArea,
			Event::Subscriber(&RepairWindow::HandleMouseLeaveSlotArea, this));
		repairSlot_->subscribeEvent(Window::EventMouseClick,
			Event::Subscriber(&RepairWindow::HandleSlotMouseClickSingle, this));
		repairSlot_->subscribeEvent(Window::EventMouseDoubleClick,
			Event::Subscriber(&RepairWindow::HandleSlotMouseDoubleClick, this));
		repairSlot_->subscribeEvent(Window::EventDragDropItemDropped,
			Event::Subscriber(&RepairWindow::HandleRepairDragDropItemDropped, this));

		dragItem_ = createItemDragItem("Repair/DragItem");

		dragContainer_ = createItemDragContainer("Repair/DragContainer", dragItem_);
		dragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&RepairWindow::HandleDragStarted, this));
		dragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&RepairWindow::HandleDragEnded, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "RepairWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

RepairWindow::~RepairWindow()
{
	dragItem_->destroy();
	dragContainer_->destroy();
	window_->destroy();
}

bool RepairWindow::HandleShown(const EventArgs& e)
{
	setItemSlot(*repairSlot_, nullptr);
	inventoryScrollBar_->setScrollPosition(0);
	window_->moveToFront();
	window_->setModalState(true);
	Update();
	return true;
}

bool RepairWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool RepairWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

void RepairWindow::Update()
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (attributeComponent == nullptr || inventoryComponent == nullptr)
	{
		return;
	}

	money_->setText(PropertyHelper::uintToString(attributeComponent->GetMoney()));

	ItemPtrArray().swap(inventoryItems_);
	for (unsigned int i = EQUIPMENT_SLOT_COUNT; i < inventoryComponent->GetInventoryItemSize(); ++i)
	{
		if (!inventoryComponent->GetInventoryItem(i).Empty())
		{
			if (inventoryComponent->GetInventoryItem(i).GetType() <= ItemType::Accessory)
			{
				inventoryComponent->GetInventoryItem(i).SetIndex(i);
				inventoryItems_.push_back(&inventoryComponent->GetInventoryItem(i));
			}
		}
	}

	const float scrollPosition = inventoryScrollBar_->getScrollPosition();
	const float stepSize	   = inventoryScrollBar_->getStepSize();
	const float documentSize   = stepSize * std::ceilf(inventoryItems_.size() / stepSize);

	inventoryScrollBar_->setDocumentSize(documentSize);
	inventoryScrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	const unsigned int startPosition = static_cast<const unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < inventoryItems_.size() && repairSlot_->getUserData() != inventoryItems_[index])
		{
			setItemSlot(*inventorySlots_[i], inventoryItems_[index]);
		}
		else
		{
			setItemSlot(*inventorySlots_[i], nullptr);
		}
	}
}

bool RepairWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool RepairWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	return handleMouseEnterItemSlotArea(dragContainer_, dragItem_, e);
}

bool RepairWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

bool RepairWindow::HandleSlotMouseClickSingle(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	return HandleSlotMouseClick(e, false);
}

bool RepairWindow::HandleSlotMouseDoubleClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	return HandleSlotMouseClick(e, true);
}

bool RepairWindow::HandleSlotMouseClick(const EventArgs& e, const bool doubleClick)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if ((doubleClick && mouseEventArgs.button == LeftButton) || mouseEventArgs.button == RightButton)
	{
		UpdateRepairSlot(*mouseEventArgs.window);
	}
	return true;
}

bool RepairWindow::HandleScrollInventory(const EventArgs& e)
{
	return handleItemScroll(*inventoryScrollBar_, e, "Repair/InventorySlot");
}

bool RepairWindow::HandleRepairButtonClicked(const EventArgs& e)
{
	const ItemAttributeComponent* item = static_cast<const ItemAttributeComponent*>(repairSlot_->getUserData());
	if (item != nullptr && !item->Empty())
	{
		if (item->GetDura() >= item->GetMaximumDura())
		{
			Game::Instance().messageBoxWindow->Show("The item does not need to be repaired.");
		}
		else
		{
			SendRepairItem(item->GetIndex());
		}
	}
	return true;
}

bool RepairWindow::HandleInventoryChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool RepairWindow::HandleDragStarted(const EventArgs& e)
{
	if (processing_)
	{
		dragContainer_->setDraggingEnabled(false);
		return true;
	}
	return handleItemDragStarted(*dragContainer_, *dragItem_, e);
}

bool RepairWindow::HandleDragEnded(const EventArgs& e)
{
	return handleItemDragEnded(*dragItem_, e);
}

bool RepairWindow::HandleInventoryDragDropItemDropped(const EventArgs& e)
{
    const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem == dragContainer_ && dragDropArgs.dragDropItem->getParent() == repairSlot_)
	{
		UpdateRepairSlot(*repairSlot_);
	}
	return true;
}

bool RepairWindow::HandleRepairDragDropItemDropped(const EventArgs& e)
{
    const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem == dragContainer_ &&
		dragDropArgs.dragDropItem->getParent() == inventorySlots_[dragDropArgs.dragDropItem->getID()])
	{
		UpdateRepairSlot(*inventorySlots_[dragDropArgs.dragDropItem->getID()]);
	}
	return true;
}

void RepairWindow::UpdateRepairSlot(Window& window)
{
	handleItemTooltip(nullptr);

	// Either remove the item from the repair slot, therefore showing the item
	// within the inventory list again or place item into repair slot and hide
	// the item within the inventory list.
	if (&window == repairSlot_)
	{
		setItemSlot(*repairSlot_, nullptr);
		Update();
	}
	else
	{
		ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(window.getUserData());
		if (item != nullptr && !item->Empty())
		{
			repairSlot_->setID(window.getID());
			setItemSlot(*repairSlot_, item);

			unsigned int repairDuraAfter = static_cast<unsigned int>(item->GetMaximumDura() * 0.95);
			unsigned int repairCost = static_cast<unsigned int>(static_cast<double>(item->GetMaximumDura() - item->GetDura()) /
				item->GetOriginalMaximumDura() * item->GetMoney() * 0.5);
			if (repairCost == 0 && item->GetMaximumDura() != item->GetDura())
			{
				repairCost = 1;
			}

			item->SetRepairDuraAfter(repairDuraAfter);
			item->SetRepairCost(repairCost);

			Update();
		}
	}
}

void RepairWindow::SendRepairItem(short index)
{
	GamePacket packet(GameProtocol::PKT_REPAIR_REQ);
	packet << static_cast<sf::Uint16>(index);
	Game::Instance().gameSocket->Send(packet);

	processing_ = true;
	setItemSlot(*repairSlot_, nullptr);
}

void RepairWindow::HandleRepairItem(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		Game::Instance().messageBoxWindow->Show("Good News! I was able to repair the item this time!");
	}
	else
	{
		sf::Uint8 subResult;
		packet >> subResult;

		std::string message;
		switch (subResult)
		{
		case GameProtocol::FAIL_NONEED:
			message = "The item does not need to be repaired.";
			break;
		case GameProtocol::FAIL_NOREPAIR:
			message = "The item was not repaired.";
			break;
		case GameProtocol::FAIL_NOMONEY:
			message = "Im Sorry, But you do not have the required funds for me to repair this item.";
			break;
		case GameProtocol::FAIL_DESTROY:
			message = "Im Sorry, But i kinda broke your item...";
			break;
		case GameProtocol::FAIL_REPAIR: // Fall-through!
		default:
			message = "Im Sorry, But the repair failed...";
			break;
		}

		Game::Instance().messageBoxWindow->Show(message);
	}
}

} // namespace GUI