
#include "GUI/InventoryWindow.h"
#include "GUI/HUD.h"
#include "GUI/InputBoxWindow.h"
#include "GUI/Common/Item.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include "Components/AttributeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/ItemAttributeComponent.h"
#include "Components/PositionComponent.h"

#include "Framework/GUI.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

String InventoryWindow::EventInventoryChanged = "EventInventoryChanged";

InventoryWindow::InventoryWindow() :
inventorySlotUpdating(false),
quickFromIndex(-1),
inventoryToIndex(-1),
equipEffectAnimationInstance_(nullptr),
characterRender_("CharacterRender", "texture")
{
	try
	{	
		window_ = WindowManager::getSingleton().loadWindowLayout("inventorywindow.layout");
		window_->hide();
		window_->addEvent(EventInventoryChanged);
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&InventoryWindow::HandleScrollInventory, this));
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&InventoryWindow::HandleOpen, this));
		window_->subscribeEvent(EventInventoryChanged, Event::Subscriber(&InventoryWindow::HandleInventoryChanged, this));
		

		AnimationManager::getSingletonPtr()->loadAnimationsFromXML("inventorywindow.animation");

		characterRenderArea_ = window_->getChild("Inventory/CharacterRenderArea");
		characterRenderArea_->subscribeEvent(Window::EventDragDropItemDropped,
			Event::Subscriber(&InventoryWindow::HandleCharacterDragDropItemDropped, this));

		characterRenderImage_ = characterRenderArea_->getChild("Inventory/CharacterRenderArea/Image");
		characterRenderImage_->setProperty("Image", "set:CharacterRender image:texture");
		characterRenderImage_->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&InventoryWindow::HandleCharacterRenderUpdated, this));

		equipmentSlots_[0] = window_->getChild("Inventory/EquipHelmet");
		equipmentSlots_[1] = window_->getChild("Inventory/EquipNecklaceEarring1");
		equipmentSlots_[2] = window_->getChild("Inventory/EquipNecklaceEarring2");
		equipmentSlots_[3] = window_->getChild("Inventory/EquipArmorTop");
		equipmentSlots_[4] = window_->getChild("Inventory/EquipArmorBottom");
		equipmentSlots_[5] = window_->getChild("Inventory/EquipBoots");
		equipmentSlots_[6] = window_->getChild("Inventory/EquipLeftHand");
		equipmentSlots_[7] = window_->getChild("Inventory/EquipRightHand");
		equipmentSlots_[8] = window_->getChild("Inventory/EquipRing1");
		equipmentSlots_[9] = window_->getChild("Inventory/EquipRing2");

		for (int i = 0; i < EQUIPMENT_SLOT_COUNT; ++i)
		{
			equipmentSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&InventoryWindow::HandleMouseEnterSlotArea, this));
			equipmentSlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&InventoryWindow::HandleMouseLeaveSlotArea, this));
			equipmentSlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&InventoryWindow::HandleEquipmentDragDropItemDropped, this));
			equipmentSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&InventoryWindow::HandleEquipmentMouseClick, this));
			equipmentSlots_[i]->subscribeEvent(Window::EventMouseDoubleClick,
				Event::Subscriber(&InventoryWindow::HandleEquipmentMouseDoubleClick, this));
		}

		for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
		{
			inventorySlots_[i] = window_->getChild("Inventory/Slot" + PropertyHelper::intToString(i+1));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&InventoryWindow::HandleMouseEnterSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&InventoryWindow::HandleMouseLeaveSlotArea, this));
			inventorySlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&InventoryWindow::HandleInventoryDragDropItemDropped, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&InventoryWindow::HandleScrollInventory, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&InventoryWindow::HandleInventoryMouseClick, this));
			inventorySlots_[i]->subscribeEvent(Window::EventMouseDoubleClick,
				Event::Subscriber(&InventoryWindow::HandleInventoryMouseDoubleClick, this));
		}

		scrollBar_ = static_cast<Scrollbar*>(window_->getChild("Inventory/Scrollbar"));
		scrollBar_->setStepSize(5.0f);
		scrollBar_->setPageSize(static_cast<float>(INVENTORY_SLOT_COUNT));
		scrollBar_->setOverlapSize(0);
		scrollBar_->setDocumentSize(0);
		scrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&InventoryWindow::HandleScrollPositionChanged, this));

		moneyButton_ = static_cast<PushButton*>(window_->getChild("Inventory/MoneyButton"));
		moneyButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&InventoryWindow::HandleThrowMoney, this));

		physicalDamage_ = window_->getChild("Inventory/PhysicalDamage");
		physicalDefense_ = window_->getChild("Inventory/PhysicalDefense");
		magicDamage_ = window_->getChild("Inventory/MagicDamage");
		magicDefense_ = window_->getChild("Inventory/MagicDefense");
		money_ = window_->getChild("Inventory/Money");

		closeButton_ = static_cast<PushButton*>(window_->getChild("Inventory/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&InventoryWindow::HandleClose, this));

		dragItem_ = createItemDragItem("Inventory/DragItem");

		dragContainer_ = createItemDragContainer("Inventory/DragContainer", dragItem_);
		dragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&InventoryWindow::HandleDragStarted, this));
		dragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&InventoryWindow::HandleDragEnded, this));
		dragContainer_->subscribeEvent(DragContainer::EventMouseButtonUp,
			Event::Subscriber(&InventoryWindow::HandleDragMouseButtonUp, this));

		blockedSlot_ = window_->getChild("Inventory/BlockedSlot");
		window_->removeChildWindow(blockedSlot_);

		equipEffect_ = window_->getChild("Inventory/EquipEffect");
		window_->removeChildWindow(equipEffect_);

		equipEffectAnimationInstance_ = AnimationManager::getSingletonPtr()->instantiateAnimation("EquipEffect");
		equipEffectAnimationInstance_->setTargetWindow(equipEffect_);

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "InventoryWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

InventoryWindow::~InventoryWindow()
{
	AnimationManager::getSingletonPtr()->destroyAnimation("EquipEffect");
	equipEffect_->destroy();
	blockedSlot_->destroy();
	dragItem_->destroy();
	dragContainer_->destroy();
	window_->destroy();
}

bool InventoryWindow::HandleOpen(const EventArgs& e)
{
	Update();
	return true;
}

bool InventoryWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool InventoryWindow::HandleCharacterRenderUpdated(const EventArgs& e)
{
	const float deltaTime = static_cast<const UpdateEventArgs&>(e).d_timeSinceLastFrame;
	characterRender_.Update(*characterRenderImage_, deltaTime);
	return true;
}

void InventoryWindow::Update()
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (attributeComponent == nullptr || inventoryComponent == nullptr)
	{
		return;
	}

	characterRender_.SetBody(attributeComponent->GetGender(), attributeComponent->GetSkin(), attributeComponent->GetHair(),
		attributeComponent->GetClassType() != 0);

	money_->setText(PropertyHelper::uintToString(attributeComponent->GetMoney()));

	for (int i = 0; i < EQUIPMENT_SLOT_COUNT; ++i)
	{
		ItemAttributeComponent& item = inventoryComponent->GetInventoryItem(i);
		item.SetIndex(i);
		equipmentSlots_[i]->setUserData(&item);
		if (!item.Empty())
		{
			equipmentSlots_[i]->setProperty("Image", getItemImageString(item));
			characterRender_.SetEquipment(i, item.GetPictureId());
		}
		else
		{
			characterRender_.SetEquipment(i, 0);

			switch (i)
			{
			case 0:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipHelmet");
				break;
			case 1:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipNecklaceEarring");
				break;
			case 2:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipNecklaceEarring");
				break;
			case 3:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipArmorTop");
				break;
			case 4:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipArmorBottom");
				break;
			case 5:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipBoots");
				break;
			case 6:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipHand");
				break;
			case 7:
				if (!inventoryComponent->GetInventoryItem(ItemEquipPosition::RightHand).Empty() &&
					inventoryComponent->GetInventoryItem(ItemEquipPosition::RightHand).GetEquipPosition() == ItemEquipPosition::BothHands)
				{
					equipmentSlots_[i]->setProperty("Image", equipmentSlots_[ItemEquipPosition::RightHand]->getProperty("Image"));
					equipmentSlots_[i]->addChildWindow(blockedSlot_);
				}
				else
				{
					equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipHand");
					equipmentSlots_[i]->removeChildWindow(blockedSlot_);
				}
				break;
			case 8:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipRing");
				break;
			case 9:
				equipmentSlots_[i]->setProperty("Image", "set:inventorywindow image:EquipRing");
				break;
			}
		}
	}

	const float scrollPosition = scrollBar_->getScrollPosition();
	const float stepSize	   = scrollBar_->getStepSize();
	const float documentSize   =
		stepSize * std::ceilf((inventoryComponent->GetInventoryItemSize() - EQUIPMENT_SLOT_COUNT) / stepSize);

	scrollBar_->setDocumentSize(documentSize);
	scrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	const unsigned int startPosition = static_cast<const unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < INVENTORY_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition+i+EQUIPMENT_SLOT_COUNT;
		inventoryComponent->GetInventoryItem(index).SetIndex(index);
		setItemSlot(*inventorySlots_[i], &inventoryComponent->GetInventoryItem(index));
	}
}

bool InventoryWindow::HandleScrollInventory(const EventArgs& e)
{
	return handleItemScroll(*scrollBar_, e, "Inventory/Slot");
}

bool InventoryWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool InventoryWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	return handleMouseEnterItemSlotArea(dragContainer_, dragItem_, e);
}

bool InventoryWindow::HandleMouseLeaveSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

bool InventoryWindow::HandleInventoryDragDropItemDropped(const EventArgs& e)
{
    const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	const ItemAttributeComponent* itemFrom = static_cast<ItemAttributeComponent*>(dragDropArgs.dragDropItem->getUserData());
	const ItemAttributeComponent* itemTo = static_cast<ItemAttributeComponent*>(dragDropArgs.window->getUserData());
	if (dragDropArgs.dragDropItem == dragContainer_)
	{
		if (itemFrom != nullptr && itemTo != nullptr && itemFrom->GetIndex() != itemTo->GetIndex())
		{
			SendChangeItem(itemFrom->GetIndex(), itemTo->GetIndex());
		}
		updateItemDragContainer(*dragDropArgs.window, *dragContainer_, *dragItem_);
	}
	else if (dragDropArgs.dragDropItem->getName().compare("Hud/ItemDragContainer") == 0)
	{
		if (itemFrom != nullptr && itemTo != nullptr && !itemFrom->Empty())
		{
			SendItemToInv(itemFrom->GetIndex(), itemTo->GetIndex());
		}
	}
	return true;
}

bool InventoryWindow::HandleEquipmentDragDropItemDropped(const EventArgs& e)
{
    const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem != dragContainer_)
	{
		return true;
	}

	const ItemAttributeComponent* itemFrom = static_cast<ItemAttributeComponent*>(dragDropArgs.dragDropItem->getUserData());
	const ItemAttributeComponent* itemTo = static_cast<ItemAttributeComponent*>(dragDropArgs.window->getUserData());
	if (itemFrom != nullptr && itemTo != nullptr && itemFrom->GetIndex() != itemTo->GetIndex())
	{
		bool canSwapItem = false;
		if (itemFrom->GetIndex() >= 10 && itemFrom->GetType() <= ItemType::Accessory)
		{
			if (itemFrom->GetEquipPosition() == ItemEquipPosition::BothHands)
			{
				if (itemTo->GetIndex() == ItemEquipPosition::RightHand)
				{
					canSwapItem = true;
				}
			}
			else if (itemFrom->GetEquipPosition() == ItemEquipPosition::Wrists)
			{
				if (itemTo->GetIndex() == 8 || itemTo->GetIndex() == 9)
				{
					canSwapItem = true;
				}
			}
			else
			{
				if (itemTo->GetIndex() == itemFrom->GetEquipPosition())
				{
					canSwapItem = true;
				}
			}

			if (canSwapItem)
			{
				SendChangeItem(itemFrom->GetIndex(), itemFrom->GetEquipPosition());
			}
		}
	}
	updateItemDragContainer(*dragDropArgs.window, *dragContainer_, *dragItem_);
	return true;
}

void InventoryWindow::EquipOrUseItem(CEGUI::Window& window)
{
	const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(window.getUserData());
	if (item == nullptr || item->Empty())
	{
		return;
	}

	if (item->GetEquipPosition() == ItemEquipPosition::Use)
	{
		SendUseItem(item->GetIndex());
	}
	else if (item->GetEquipPosition() >= ItemEquipPosition::Head && item->GetEquipPosition() <= ItemEquipPosition::Wrists &&
	         item->GetType() <= ItemType::Accessory)
	{
		SendChangeItem(item->GetIndex(), item->GetEquipPosition());
	}
}

void InventoryWindow::UnequipItem(CEGUI::Window& window)
{
	const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(window.getUserData());
	if (item != nullptr && !item->Empty())
	{
		SendChangeItem(item->GetIndex());
	}
}

bool InventoryWindow::HandleCharacterDragDropItemDropped(const EventArgs& e)
{
    const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem->getName().compare("Inventory/DragContainer") != 0)
	{
		return true;
	}
	EquipOrUseItem(*dragDropArgs.dragDropItem);
	updateItemDragContainer(*dragDropArgs.window, *dragContainer_, *dragItem_);
	return true;
}

bool InventoryWindow::HandleDragStarted(const EventArgs& e)
{
	if (inventorySlotUpdating)
	{
		dragContainer_->setDraggingEnabled(false);
		return true;
	}
	return handleItemDragStarted(*dragContainer_, *dragItem_, e);
}

bool InventoryWindow::HandleDragEnded(const EventArgs& e)
{
	return handleItemDragEnded(*dragItem_, e);
}

bool InventoryWindow::HandleDragMouseButtonUp(const EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		const Window* hudFrame = System::getSingleton().getGUISheet()->getChild("Hud/Frame");
		if (!hudFrame->isHit(mouseEventArgs.position) && !window_->isHit(mouseEventArgs.position))
		{
			ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(mouseEventArgs.window->getUserData());
			if (item != nullptr && !item->Empty())
			{
				// Do the same thing as HandleDragEnded because for some reason the event does not get called (maybe because mouse outside window).
				// Not setting the alpha here for the dragItem will result in it still being seen.
				dragItem_->setAlpha(0.0f);

				if (item->GetQuantity() > 1)
				{
					Game::Instance().inputBoxWindow->Show("How Many?",
						Event::Subscriber(&InventoryWindow::HandleThrowItemInputResult, this));
					Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(item->GetQuantity()));
					Game::Instance().inputBoxWindow->SetUserData(item);
				}
				else
				{
					SendThrowItem(item->GetIndex(), 1);
				}
			}
		}
	}
	return true;
}

void InventoryWindow::SendChangeItem(const short fromIndex, short toIndex)
{
	if (fromIndex < 10)
	{
		toIndex = -1;
	}

	if (toIndex >= 0 && toIndex < 10)
	{
		if (toIndex == ItemEquipPosition::BothHands)
		{
			toIndex = 6;
		}
		else if (toIndex == ItemEquipPosition::Wrists)
		{
			const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(equipmentSlots_[8]->getUserData());
			if (item != nullptr && item->Empty())
			{
				toIndex = 8;
			}
			else
			{
				toIndex = 9;
			}
		}
	}

	GamePacket packet(GameProtocol::PKT_CHANGE_ITEM_INDEX);
	packet << fromIndex << toIndex;
	Game::Instance().gameSocket->Send(packet);
}

bool InventoryWindow::HandleInventoryChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool InventoryWindow::HandleInventoryMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == RightButton)
	{
		EquipOrUseItem(*mouseEventArgs.window);
	}
	return true;
}

bool InventoryWindow::HandleInventoryMouseDoubleClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		EquipOrUseItem(*mouseEventArgs.window);
	}
	return true;
}

bool InventoryWindow::HandleEquipmentMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == RightButton)
	{
		UnequipItem(*mouseEventArgs.window);
	}
	return true;
}

bool InventoryWindow::HandleEquipmentMouseDoubleClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		UnequipItem(*mouseEventArgs.window);
	}
	return true;
}

void InventoryWindow::SendUseItem(const unsigned short index)
{
	GamePacket packet(GameProtocol::PKT_ITEM_USE);
	packet << static_cast<sf::Uint8>(1) << index; // TODO: BASIC_INV, need types!
	Game::Instance().gameSocket->Send(packet);
}

void InventoryWindow::SendItemToInv(const unsigned short fromIndex, const unsigned short toIndex)
{
	if (!inventorySlotUpdating)
	{
		quickFromIndex = fromIndex;
		inventoryToIndex = toIndex;
		inventorySlotUpdating = true;

		GamePacket packet(GameProtocol::PKT_PUTITEM_INV);
		packet << fromIndex << toIndex;
		Game::Instance().gameSocket->Send(packet);
	}
}

void InventoryWindow::SendThrowItem(const unsigned short index, const unsigned short count)
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	PositionComponent* positionComponent = Game::Instance().myEntity->getComponent<PositionComponent>();
	if (attributeComponent != nullptr && positionComponent != nullptr)
	{
		GamePacket packet(GameProtocol::PKT_ITEM_THROW);
		packet << static_cast<sf::Uint8>(1)
			   << index
			   << count
			   << static_cast<short>(positionComponent->GetCellPosition().x)
			   << static_cast<short>(positionComponent->GetCellPosition().y);
		Game::Instance().gameSocket->Send(packet);
	}
}

bool InventoryWindow::HandleThrowMoney(const EventArgs& e)
{
	Game::Instance().inputBoxWindow->Show("How Much?", Event::Subscriber(&InventoryWindow::HandleThrowMoneyInputResult, this));
	return true;
}

void InventoryWindow::SendThrowMoney(const unsigned int money)
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent != nullptr && attributeComponent->GetMoney() >= money)
	{
		PositionComponent* positionComponent = Game::Instance().myEntity->getComponent<PositionComponent>();
		if (positionComponent != nullptr)
		{
			GamePacket packet(GameProtocol::PKT_ITEM_DUMP);
			packet << money
				   << static_cast<short>(positionComponent->GetCellPosition().x)
				   << static_cast<short>(positionComponent->GetCellPosition().y);
			Game::Instance().gameSocket->Send(packet);
		}
	}
}

void InventoryWindow::StartEquipEffect(Window& equipmentSlot)
{
	// TODO: Not sure if need to cleanup anything for the animation instance
	// or if should be using a shared instance and therefore make it a member of the InventoryWindow class.
	equipmentSlot.addChildWindow(equipEffect_);
	equipEffectAnimationInstance_->start();
}

bool InventoryWindow::HandleThrowMoneyInputResult(const EventArgs& e)
{
	const unsigned int money = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (money > 0)
	{
		SendThrowMoney(money);
	}
	return true;
}

bool InventoryWindow::HandleThrowItemInputResult(const EventArgs& e)
{
	const unsigned int howMany = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (howMany > 0)
	{
		const ItemAttributeComponent* item = static_cast<const ItemAttributeComponent*>(Game::Instance().inputBoxWindow->GetUserData());
		if (item != nullptr && !item->Empty() && item->GetDura() >= howMany)
		{
			SendThrowItem(item->GetIndex(), howMany);
		}
	}
	return true;
}

void InventoryWindow::HandlePutItemInv(GamePacket& packetRecv)
{
	inventorySlotUpdating = false;

	sf::Uint8 result;
	packetRecv >> result;

	if (result == 1 || result == 3)
	{
		InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
		if (inventoryComponent != nullptr)
		{
			if (result == 1)
			{
				std::swap(inventoryComponent->GetBeltItem(quickFromIndex), inventoryComponent->GetInventoryItem(inventoryToIndex));
			}
			else if (result == 3)
			{
				inventoryComponent->GetBeltItem(quickFromIndex).SetDura(0);
			}

			Game::Instance().gui->FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("Shop/Frame", GUI::InventoryWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("PosShop/Frame", GUI::InventoryWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("Repair/Frame", GUI::InventoryWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("Stash/Frame", GUI::InventoryWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("GuildStash/Frame", GUI::InventoryWindow::EventInventoryChanged);

			Game::Instance().gui->FireEvent("Hud/Frame", GUI::HUD::EventQuickSlotChanged);
		}
	}
}

void InventoryWindow::HandleChangeItemIndex(GamePacket& packetRecv)
{
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (inventoryComponent == nullptr)
	{
		return;
	}

	sf::Uint8 result;
	packetRecv >> result;

	if (result == GameProtocol::SUCCESS)
	{
		sf::Uint16 fromIndex;
		packetRecv >> fromIndex;

		sf::Uint16 toIndex;
		packetRecv >> toIndex;

		std::swap(inventoryComponent->GetInventoryItem(fromIndex), inventoryComponent->GetInventoryItem(toIndex));

		if (toIndex >= 0 && toIndex < 10)
		{
			StartEquipEffect(*equipmentSlots_[toIndex]);
		}

		Game::Instance().gui->FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
		Game::Instance().gui->FireEvent("Shop/Frame", GUI::InventoryWindow::EventInventoryChanged);
		Game::Instance().gui->FireEvent("PosShop/Frame", GUI::InventoryWindow::EventInventoryChanged);
		Game::Instance().gui->FireEvent("Repair/Frame", GUI::InventoryWindow::EventInventoryChanged);
		Game::Instance().gui->FireEvent("Stash/Frame", GUI::InventoryWindow::EventInventoryChanged);
		Game::Instance().gui->FireEvent("GuildStash/Frame", GUI::InventoryWindow::EventInventoryChanged);
	}
}

} // namespace GUI