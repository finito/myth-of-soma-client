
#include "GUI/AbilityWindow.h"
#include "GUI/Common/Item.h"
#include "GUI/InputBoxWindow.h"

#include "Components/AttributeComponent.h"
#include "Components/InventoryComponent.h"

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

AbilityWindow::AbilityWindow() :
processing_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("abilitywindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&AbilityWindow::HandleOpen, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&AbilityWindow::HandleScrollInventory, this));

		resultDescription_  = window_->getChild("Ability/ResultDescription");

		resultSlot_  = window_->getChild("Ability/ResultSlot");
		resultSlot_->subscribeEvent(Window::EventMouseEntersArea,
			Event::Subscriber(&AbilityWindow::HandleMouseEnterSlotArea, this));
		resultSlot_->subscribeEvent(Window::EventMouseLeavesArea,
			Event::Subscriber(&AbilityWindow::HandleMouseLeaveSlotArea, this));

		okButton_ = static_cast<PushButton*>(window_->getChild("Ability/OKButton"));
		okButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&AbilityWindow::HandleClose, this));

		produceButton_ = static_cast<PushButton*>(window_->getChild("Ability/ProduceButton"));
		produceButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&AbilityWindow::HandleProduceButtonClicked, this));

		separationButton_ = static_cast<RadioButton*>(window_->getChild("Ability/SeparationButton"));
		compositionButton_ = static_cast<RadioButton*>(window_->getChild("Ability/CompositionButton"));
		partitionButton_ = static_cast<RadioButton*>(window_->getChild("Ability/PartitionButton"));
		upgradeButton_ = static_cast<RadioButton*>(window_->getChild("Ability/UpgradeButton"));

		inventoryScrollBar_ = static_cast<Scrollbar*>(window_->getChild("Ability/InventoryScrollbar"));
		inventoryScrollBar_->setStepSize(5.0f);
		inventoryScrollBar_->setPageSize(static_cast<float>(INVENTORY_SLOT_COUNT));
		inventoryScrollBar_->setOverlapSize(0);
		inventoryScrollBar_->setDocumentSize(0);
		inventoryScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&AbilityWindow::HandleScrollPositionChanged, this));

		for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
		{
			inventorySlots_[i] = window_->getChild("Ability/InventorySlot" + PropertyHelper::intToString(i+1));
			inventorySlots_[i]->setID(i);
			inventorySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&AbilityWindow::HandleMouseEnterSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&AbilityWindow::HandleMouseLeaveSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&AbilityWindow::HandleScrollInventory, this));
			inventorySlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&AbilityWindow::HandleInventorySlotDragDropItemDropped, this));
		}

		materialSlotArea_ = window_->getChild("Ability/MaterialSlotArea");
		materialSlotArea_->subscribeEvent(Window::EventDragDropItemDropped,
			Event::Subscriber(&AbilityWindow::HandleMaterialSlotDragDropItemDropped, this));

		for (int i = 0; i < MATERIAL_ITEM_COUNT; ++i)
		{
			materialSlots_[i] = window_->getChild("Ability/MaterialSlot" + PropertyHelper::intToString(i+1));
			materialSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&AbilityWindow::HandleMouseEnterSlotArea, this));
			materialSlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&AbilityWindow::HandleMouseLeaveSlotArea, this));
			materialSlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&AbilityWindow::HandleMaterialSlotDragDropItemDropped, this));
		}

		dragItem_ = createItemDragItem("Ability/DragItem");

		dragContainer_ = createItemDragContainer("Ability/DragContainer", dragItem_);
		dragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&AbilityWindow::HandleDragStarted, this));
		dragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&AbilityWindow::HandleDragEnded, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "AbilityWindow: Failed to initialize: " << e.what() << std::endl;
	}
}


AbilityWindow::~AbilityWindow()
{
	dragItem_->destroy();
	dragContainer_->destroy();
	window_->destroy();
}

void AbilityWindow::Update()
{
	//TODO: Use Component Mapper?
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
}

bool AbilityWindow::HandleOpen(const EventArgs& e)
{
	for (int i = 0; i < MATERIAL_ITEM_COUNT; ++i)
	{
		setItemSlot(*materialSlots_[i], nullptr);
	}

	setItemSlot(*resultSlot_, nullptr);

	resultDescription_->setText("");

	inventoryScrollBar_->setScrollPosition(0);

	window_->moveToFront();
	window_->setModalState(true);

	Update();
	return true;
}

bool AbilityWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	window_->setModalState(false);
	return true;
}

bool AbilityWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool AbilityWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	return handleMouseEnterItemSlotArea(dragContainer_, dragItem_, e);
}

bool AbilityWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

bool AbilityWindow::HandleScrollInventory(const EventArgs& e)
{
	return handleItemScroll(*inventoryScrollBar_, e, "Ability/InventorySlot");
}

bool AbilityWindow::HandleDragStarted(const EventArgs& e)
{
	if (processing_ || dragContainer_->getParent() == resultSlot_)
	{
		dragContainer_->setDraggingEnabled(false);
		return true;
	}
	return handleItemDragStarted(*dragContainer_, *dragItem_, e);
}

bool AbilityWindow::HandleDragEnded(const EventArgs& e)
{
	return handleItemDragEnded(*dragItem_, e);
}

void AbilityWindow::UpdateItemData()
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

	ItemArray().swap(materialItems_);
	materialItems_.resize(MATERIAL_ITEM_COUNT);
}

void AbilityWindow::HandleAbilityOpen(GamePacket& packet)
{
	separationButton_->disable();
	compositionButton_->disable();
	partitionButton_->disable();
	upgradeButton_->disable();

	packet >> abilityType_;

	switch (abilityType_)
	{
	case GameProtocol::ABILITY_DISASSEMBLE:
		separationButton_->enable();
		separationButton_->setSelected(true);
		abilityTypeString_ = "Disassemble";
		break;
	case GameProtocol::ABILITY_PREPARE: // Fall-through!
	case GameProtocol::ABILITY_SYNTHESIS:
		compositionButton_->enable();
		compositionButton_->setSelected(true);
		abilityTypeString_ = "Mix";
		break;
	case GameProtocol::ABILITY_SMELT:
		partitionButton_->enable();
		partitionButton_->setSelected(true);
		abilityTypeString_ = "Smelt";
		break;
	case GameProtocol::ABILITY_UPGRADE:
		upgradeButton_->enable();
		upgradeButton_->setSelected(true);
		abilityTypeString_ = "Upgrade";
		break;
	default:
		return;
	}

	UpdateItemData();

	processing_ = false;

	window_->show();
}

void AbilityWindow::MoveItemToMaterialSlot(ItemAttributeComponent& item, unsigned short count)
{
	switch (abilityType_)
	{
	case GameProtocol::ABILITY_PREPARE: // Fall-through!
	case GameProtocol::ABILITY_SYNTHESIS:
		{
			if (count == 0)
			{
				Game::Instance().inputBoxWindow->Show("How Many?",
					Event::Subscriber(&AbilityWindow::HandleAddItemInputResult, this));
				Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(1));
				Game::Instance().inputBoxWindow->SetUserData(&item);
				return;
			}

			auto it = std::find_if(materialItems_.begin(), materialItems_.end(),
				[=](ItemAttributeComponent& materialItem) { return materialItem.GetIndex() == item.GetIndex(); });

			if (it != materialItems_.end())
			{
				(*it).SetDura((*it).GetDura() + count);
			}
			else
			{
				for (int i = 0; i < MATERIAL_ITEM_COUNT; ++i)
				{
					if (materialItems_[i].Empty())
					{
						materialItems_[i] = item;
						materialItems_[i].SetDura(count);
						item.SetDura(item.GetDura() - count);
						break;
					}
				}
			}
		}
		break;
	case GameProtocol::ABILITY_DISASSEMBLE:
	case GameProtocol::ABILITY_SMELT:
	case GameProtocol::ABILITY_UPGRADE:
		{
			if (item.GetType() == ItemType::Material)
			{
				if (materialItems_[4].Empty())
				{
					materialItems_[4] = item;
					materialItems_[4].SetDura(1);
					item.SetDura(item.GetDura() - 1);
				}
			}
			else
			{
				if (count == 0)
				{
					Game::Instance().inputBoxWindow->Show("How Many?",
						Event::Subscriber(&AbilityWindow::HandleAddItemInputResult, this));
					Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(1));
					Game::Instance().inputBoxWindow->SetUserData(&item);
					return;
				}

				if (materialItems_[0].Empty())
				{
					materialItems_[0] = item;
					materialItems_[0].SetDura(count);
					item.SetDura(item.GetDura() - count);
				}
			}
		}
		break;
	}

	Update();
}

bool AbilityWindow::HandleAddItemInputResult(const EventArgs& e)
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

bool AbilityWindow::HandleMaterialSlotDragDropItemDropped(const EventArgs& e)
{
	const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem->getName().compare("Ability/DragContainer") == 0)
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
				MoveItemToMaterialSlot(*item, 0);
			}
			else
			{
				MoveItemToMaterialSlot(*item, item->GetDura());
			}
		}
	}
	return true;
}

void AbilityWindow::MoveItemToInventorySlot(const ItemAttributeComponent& item)
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

bool AbilityWindow::HandleInventorySlotDragDropItemDropped(const EventArgs& e)
{
	const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem->getName().compare("Ability/DragContainer") == 0)
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

void AbilityWindow::SendProduceSynthesis()
{
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

	GamePacket packet(GameProtocol::PKT_SKILL_ABILITY);
	packet << abilityType_ << count;

	for (int i = MATERIAL_ITEM_COUNT-1; i >= 0; --i)
	{
		if (!materialItems_[i].Empty())
		{
			packet << materialItems_[i].GetIndex() << materialItems_[i].GetDura();
		}
	}
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

void AbilityWindow::SendProduceOther()
{
	if (materialItems_[4].Empty())
	{
		resultDescription_->setText("Missing Special Item.");
		return;
	}

	if (materialItems_[0].Empty())
	{
		resultDescription_->setText("Missing Item.");
		return;
	}

	GamePacket packet(GameProtocol::PKT_SKILL_ABILITY);
	packet << abilityType_ << materialItems_[4].GetIndex() << materialItems_[0].GetIndex();
	if (abilityType_ != GameProtocol::ABILITY_DISASSEMBLE)
	{
		packet << materialItems_[0].GetDura();
	}
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

void AbilityWindow::SendProduce()
{
	switch (abilityType_)
	{
	case GameProtocol::ABILITY_PREPARE: // Fall-through!
	case GameProtocol::ABILITY_SYNTHESIS:
		SendProduceSynthesis();
		break;
	case GameProtocol::ABILITY_UPGRADE: // Fall-through!
	case GameProtocol::ABILITY_SMELT:
	case GameProtocol::ABILITY_DISASSEMBLE:
		SendProduceOther();
		break;
	}
}

bool AbilityWindow::HandleProduceButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		SendProduce();
	}
	return true;
}

void AbilityWindow::HandleProduceItem(GamePacket& packet)
{
	processing_ = false;

	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (inventoryComponent == nullptr)
	{
		return;
	}

	sf::Uint8 result;
	packet >> result;

	sf::Uint16 count = 1;
	if (abilityType_ != GameProtocol::ABILITY_UPGRADE)
	{
		packet >> count;
	}

	if (result == GameProtocol::SUCCESS)
	{
		if (count > 0)
		{
			sf::Uint16 index;
			packet >> index;
			if (index >= 0)
			{
				resultItem_.HandleItemReceive(packet, GameProtocol::ITEM_ME, false);
				resultItem_.SetDura(count);
				setItemSlot(*resultSlot_, &resultItem_);
				resultDescription_->setText("Succeeded at " + abilityTypeString_ + ".");
			}
		}
	}
	else
	{
		sf::Uint8 errorCode;
		packet >> errorCode;

		switch (errorCode)
		{
		case 6:
			resultDescription_->setText("Cannot " + abilityTypeString_ + " this item.");
			break;
		default:
			resultDescription_->setText("Item " + abilityTypeString_ + " failed.");
			break;
		}
		setItemSlot(*resultSlot_, nullptr);
	}

	UpdateItemData();
	Update();
}

} // namespace GUI