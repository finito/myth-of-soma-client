
#include "GUI/MakeItemWindow.h"
#include "GUI/Common/Item.h"
#include "GUI/InputBoxWindow.h"

#include "Components/AttributeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/ItemAttributeComponent.h"

#include "Network/GameProtocol.h"
#include "Network/GamePacket.h"
#include "Network/GameSocket.h"

#include "Framework/GUI.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

MakeItemWindow::MakeItemWindow() :
processing_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("makeitemwindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&MakeItemWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&MakeItemWindow::HandleHidden, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&MakeItemWindow::HandleScrollInventory, this));

		resultDescription_  = window_->getChild("MakeItem/ResultDescription");

		resultSlot_  = window_->getChild("MakeItem/ResultSlot");
		resultSlot_->subscribeEvent(Window::EventMouseEntersArea,
			Event::Subscriber(&MakeItemWindow::HandleMouseEnterSlotArea, this));
		resultSlot_->subscribeEvent(Window::EventMouseLeavesArea,
			Event::Subscriber(&MakeItemWindow::HandleMouseLeaveSlotArea, this));

		okButton_ = static_cast<PushButton*>(window_->getChild("MakeItem/OKButton"));
		okButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&MakeItemWindow::HandleClose, this));

		produceButton_ = static_cast<PushButton*>(window_->getChild("MakeItem/ProduceButton"));
		produceButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&MakeItemWindow::HandleProduceButtonClicked, this));

		genderCheckbox_ = static_cast<Checkbox*>(window_->getChild("MakeItem/GenderCheckbox"));

		name_ = window_->getChild("MakeItem/Name");
		
		nameButton_ = static_cast<PushButton*>(window_->getChild("MakeItem/NameButton"));
		nameButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&MakeItemWindow::HandleNameButtonClicked, this));

		inventoryScrollBar_ = static_cast<Scrollbar*>(window_->getChild("MakeItem/InventoryScrollbar"));
		inventoryScrollBar_->setStepSize(5.0f);
		inventoryScrollBar_->setPageSize(static_cast<float>(INVENTORY_SLOT_COUNT));
		inventoryScrollBar_->setOverlapSize(0);
		inventoryScrollBar_->setDocumentSize(0);
		inventoryScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&MakeItemWindow::HandleScrollPositionChanged, this));

		for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
		{
			inventorySlots_[i] = window_->getChild("MakeItem/InventorySlot" + PropertyHelper::intToString(i+1));
			inventorySlots_[i]->setID(i);
			inventorySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&MakeItemWindow::HandleMouseEnterSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&MakeItemWindow::HandleMouseLeaveSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&MakeItemWindow::HandleScrollInventory, this));
			inventorySlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&MakeItemWindow::HandleInventorySlotDragDropItemDropped, this));
		}

		for (int i = 0; i < TYPE_SLOT_COUNT; ++i)
		{
			typeSlots_[i] = window_->getChild("MakeItem/TypeSlot" + PropertyHelper::intToString(i+1)); 
			typeSlots_[i]->setID(-1);
		}

		for (int i = 0; i < TYPE_COUNT; ++i)
		{
			typeButtons_[i] = static_cast<RadioButton*>(window_->getChild("MakeItem/TypeButton" + PropertyHelper::intToString(i+1)));
			typeButtons_[i]->setID(-1);
			typeButtons_[i]->hide();
			typeButtons_[i]->subscribeEvent(RadioButton::EventSelectStateChanged,
				Event::Subscriber(&MakeItemWindow::HandleTypeButtonSelectStateChanged, this));
		}

		materialSlotArea_ = window_->getChild("MakeItem/MaterialSlotArea");
		materialSlotArea_->subscribeEvent(Window::EventDragDropItemDropped,
			Event::Subscriber(&MakeItemWindow::HandleMaterialSlotDragDropItemDropped, this));

		for (int i = 0; i < MATERIAL_ITEM_COUNT; ++i)
		{
			materialSlots_[i] = window_->getChild("MakeItem/MaterialSlot" + PropertyHelper::intToString(i+1));
			materialSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&MakeItemWindow::HandleMouseEnterSlotArea, this));
			materialSlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&MakeItemWindow::HandleMouseLeaveSlotArea, this));
			materialSlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&MakeItemWindow::HandleMaterialSlotDragDropItemDropped, this));
		}

		dragItem_ = createItemDragItem("MakeItem/DragItem");

		dragContainer_ = createItemDragContainer("MakeItem/DragContainer", dragItem_);
		dragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&MakeItemWindow::HandleDragStarted, this));
		dragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&MakeItemWindow::HandleDragEnded, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "MakeItemWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

MakeItemWindow::~MakeItemWindow()
{
	dragItem_->destroy();
	dragContainer_->destroy();
	window_->destroy();
}

void MakeItemWindow::Update()
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent == nullptr)
	{
		return;
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
		if (index < inventoryItems_.size())
		{
			setItemSlot(*inventorySlots_[i], &inventoryItems_[index]);
		}
		else
		{
			setItemSlot(*inventorySlots_[i], nullptr);
		}
	}

	for (int i = 0; i < MATERIAL_ITEM_COUNT; ++i)
	{
		setItemSlot(*materialSlots_[i], &materialItems_[i]);
	}

	for (int i = 0; i < TYPE_COUNT; ++i)
	{
		typeButtons_[i]->setID(-1);
		typeButtons_[i]->hide();
	}

	for (int i = 0; i < TYPE_SLOT_COUNT; ++i)
	{
		const unsigned int buttonIndex = typeSlots_[i]->getID();
		if (buttonIndex > 0 && buttonIndex <= TYPE_COUNT)
		{
			typeButtons_[buttonIndex-1]->setID(buttonIndex);
			typeButtons_[buttonIndex-1]->setArea(typeSlots_[i]->getArea());
			typeButtons_[buttonIndex-1]->show();
		}
	}
}

bool MakeItemWindow::HandleShown(const EventArgs& e)
{
	for (int i = 0; i < MATERIAL_ITEM_COUNT; ++i)
	{
		setItemSlot(*materialSlots_[i], nullptr);
	}

	setItemSlot(*resultSlot_, nullptr);

	resultDescription_->setText("");

	name_->setText("");

	inventoryScrollBar_->setScrollPosition(0);
	
	window_->moveToFront();
	window_->setModalState(true);
	
	Update();

	const unsigned int buttonIndex = typeSlots_[0]->getID();
	if (buttonIndex > 0 && buttonIndex <= TYPE_COUNT)
	{
		typeButtons_[buttonIndex-1]->setSelected(true);
	}
	return true;
}

bool MakeItemWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool MakeItemWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool MakeItemWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool MakeItemWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	return handleMouseEnterItemSlotArea(dragContainer_, dragItem_, e);
}

bool MakeItemWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

bool MakeItemWindow::HandleScrollInventory(const EventArgs& e)
{
	return handleItemScroll(*inventoryScrollBar_, e, "MakeItem/InventorySlot");
}

bool MakeItemWindow::HandleDragStarted(const EventArgs& e)
{
	if (processing_ || dragContainer_->getParent() == resultSlot_)
	{
		dragContainer_->setDraggingEnabled(false);
		return true;
	}
	return handleItemDragStarted(*dragContainer_, *dragItem_, e);
}

bool MakeItemWindow::HandleDragEnded(const EventArgs& e)
{
	return handleItemDragEnded(*dragItem_, e);
}

void MakeItemWindow::UpdateItemData()
{
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (inventoryComponent != nullptr)
	{
		ItemArray().swap(inventoryItems_);
		for (unsigned int i = EQUIPMENT_SLOT_COUNT; i < inventoryComponent->GetInventoryItemSize(); ++i)
		{
			if (!inventoryComponent->GetInventoryItem(i).Empty())
			{
				if (inventoryComponent->GetInventoryItem(i).GetType() == ItemType::Material)
				{
					inventoryComponent->GetInventoryItem(i).SetIndex(i);
					inventoryItems_.push_back(inventoryComponent->GetInventoryItem(i));
				}
			}
		}
	}

	ItemArray().swap(materialItems_);
	materialItems_.resize(MATERIAL_ITEM_COUNT);
}

void MakeItemWindow::HandleMakeItemOpen(GamePacket& packet)
{
	for (int i = 0; i < TYPE_SLOT_COUNT; ++i)
	{
		typeSlots_[i]->setID(-1);
	}

	sf::Int16 count;
	packet >> count;
	count = std::min(count, static_cast<short>(TYPE_SLOT_COUNT));

	for (int i = 0; i < count; ++i)
	{
		sf::Uint16 type;
		packet >> type;
		typeSlots_[i]->setID(type);
	}

	UpdateItemData();

	processing_ = false;

	window_->show();
}

void MakeItemWindow::MoveItemToMaterialSlot(ItemAttributeComponent& item, const unsigned short count)
{
	auto it = std::find_if(materialItems_.begin(), materialItems_.end(),
		[=](ItemAttributeComponent& materialItem) { return materialItem.GetIndex() == item.GetIndex(); });

	if (it != materialItems_.end())
	{
		(*it).SetDura((*it).GetDura() + count);
	}
	else
	{
		if (item.GetEquipPosition() == ItemMaterialType::Gem)
		{
			if (materialItems_[4].Empty())
			{
				materialItems_[4] = item;
				materialItems_[4].SetDura(count);
				item.SetDura(item.GetDura() - count);
			}
		}
		else
		{
			for (int i = 0; i < MATERIAL_ITEM_COUNT-1; ++i)
			{
				if (materialItems_[i].Empty())
				{
					materialItems_[i] = item;
					materialItems_[i].SetDura(count);
					if (item.GetQuantity() > 1)
					{
						item.SetDura(item.GetDura() - count);
					}
					else
					{
						item.SetDura(0);
					}
					break;
				}
			}
		}
	}

	Update();
}

bool MakeItemWindow::HandleAddItemInputResult(const EventArgs& e)
{
	const unsigned int howMany = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (howMany > 0)
	{
		ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(Game::Instance().inputBoxWindow->GetUserData());
		if (item != nullptr && !item->Empty() && item->GetQuantity() >= howMany)
		{
			MoveItemToMaterialSlot(*item, howMany);
		}
	}
	return true;
}

bool MakeItemWindow::HandleMaterialSlotDragDropItemDropped(const EventArgs& e)
{
	const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem->getName().compare("MakeItem/DragContainer") == 0)
	{
		String::size_type found = dragDropArgs.dragDropItem->getParent()->getName().find("InventorySlot");
		if (found == String::npos)
		{
			return true;
		}

		ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(dragDropArgs.dragDropItem->getUserData());
		if (item != nullptr && !item->Empty())
		{
			if (item->GetQuantity() > 1)
			{
				Game::Instance().inputBoxWindow->Show("How Many?",
					Event::Subscriber(&MakeItemWindow::HandleAddItemInputResult, this));
				Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(1));
				Game::Instance().inputBoxWindow->SetUserData(item);
			}
			else
			{
				MoveItemToMaterialSlot(*item, 1);
			}
		}
	}
	return true;
}

void MakeItemWindow::MoveItemToInventorySlot(const ItemAttributeComponent& item)
{
	auto it = std::find_if(inventoryItems_.begin(), inventoryItems_.end(),
		[=](ItemAttributeComponent& inventoryItem) { return inventoryItem.GetIndex() == item.GetIndex(); });

	if (it != inventoryItems_.end())
	{
		(*it).SetDura((*it).GetDura() + item.GetDura());
	}

	it = std::find_if(materialItems_.begin(), materialItems_.end(),
		[=](ItemAttributeComponent& materialItem) { return materialItem.GetIndex() == item.GetIndex(); });

	if (it != materialItems_.end())
	{
		(*it).SetDura(0);
	}

	Update();
}

bool MakeItemWindow::HandleInventorySlotDragDropItemDropped(const EventArgs& e)
{
	const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem->getName().compare("MakeItem/DragContainer") == 0)
	{
		String::size_type found = dragDropArgs.dragDropItem->getParent()->getName().find("MaterialSlot");
		if (found == String::npos)
		{
			return true;
		}

		const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(dragDropArgs.dragDropItem->getUserData());
		if (item != nullptr && !item->Empty())
		{
			MoveItemToInventorySlot(*item);
		}
	}
	return true;
}

void MakeItemWindow::SendProduce()
{
	GamePacket packet(GameProtocol::PKT_MAKEITEM);

	unsigned char count = 0;
	for (int i = 0; i < MATERIAL_ITEM_COUNT; ++i)
	{
		if (!materialItems_[i].Empty())
		{
			++count;
		}
	}

	if (count == 0)
	{
		return;
	}

	unsigned short makeItemType = typeButtons_[0]->getSelectedButtonInGroup()->getID();
	unsigned char gender = ItemGender::UniSex;
	if (!genderCheckbox_->isDisabled())
	{
		gender = genderCheckbox_->isSelected() ? ItemGender::Female : ItemGender::Male;
	}

	packet << makeItemType << gender << name_->getText().c_str() << count;
	for (int i = 0; i < MATERIAL_ITEM_COUNT; ++i)
	{
		if (!materialItems_[i].Empty())
		{
			packet << materialItems_[i].GetIndex() << materialItems_[i].GetDura();
		}
	}
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

bool MakeItemWindow::HandleProduceButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		SendProduce();
	}
	return true;
}

void MakeItemWindow::HandleProduceItem(GamePacket& packet)
{
	processing_ = false;

	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (inventoryComponent == nullptr)
	{
		return;
	}

	sf::Uint8 result;
	packet >> result;

	sf::Uint16 count;
	packet >> count;

	if (result == GameProtocol::SUCCESS)
	{
		if (count > 0)
		{
			for (int i = 0; i < count; ++i)
			{
				sf::Uint16 itemIndex;
				packet >> itemIndex;

				sf::Uint16 remainingDura;
				packet >> remainingDura;

				inventoryComponent->GetInventoryItem(itemIndex).SetIndex(itemIndex);
				inventoryComponent->GetInventoryItem(itemIndex).SetDura(remainingDura);
				Game::Instance().gui->FireEvent("Inventory/Frame", "EventInventoryChanged");
				Game::Instance().gui->FireEvent("Shop/Frame", "EventInventoryChanged");
				Game::Instance().gui->FireEvent("PosShop/Frame", "EventInventoryChanged");
				Game::Instance().gui->FireEvent("Repair/Frame", "EventInventoryChanged");
				Game::Instance().gui->FireEvent("Stash/Frame", "EventInventoryChanged");
				Game::Instance().gui->FireEvent("GuildStash/Frame", "EventInventoryChanged");
			}

			sf::Uint16 index;
			packet >> index;
			if (index >= 0)
			{
				inventoryComponent->GetInventoryItem(index).HandleItemReceive(packet, GameProtocol::ITEM_ME, false);
				setItemSlot(*resultSlot_, &inventoryComponent->GetInventoryItem(index));
				resultDescription_->setText("Item has been created.");
			}
		}
	}
	else
	{
		resultDescription_->setText("Item manufacture has failed.");
		setItemSlot(*resultSlot_, nullptr);
	}

	UpdateItemData();
	Update();
}

bool MakeItemWindow::HandleTypeButtonSelectStateChanged(const EventArgs& e)
{
	const WindowEventArgs& windowEventArgs = static_cast<const WindowEventArgs&>(e);
	RadioButton* typeButton = static_cast<RadioButton*>(windowEventArgs.window);
	if (typeButton->isSelected())
	{
		switch (typeButton->getID())
		{
		case 9: // Fall-through!
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 22:
		case 23:
			genderCheckbox_->enable();
			break;
		default:
			genderCheckbox_->disable();
			break;
		}

		if (typeButton->getID() >= 16 && typeButton->getID() <= 21)
		{
			name_->disable();
			nameButton_->disable();
		}
		else
		{
			name_->enable();
			nameButton_->enable();
		}
	}
	return true;
}

bool MakeItemWindow::HandleItemNameInputResult(const EventArgs&e)
{
	name_->setText(Game::Instance().inputBoxWindow->GetInputText());
	return true;
}

bool MakeItemWindow::HandleNameButtonClicked(const EventArgs& e)
{
	Game::Instance().inputBoxWindow->Show("Item Name", Event::Subscriber(&MakeItemWindow::HandleItemNameInputResult, this));
	Game::Instance().inputBoxWindow->SetInputText(name_->getText());
	return true;
}

} // namespace GUI