
#include "GUI/HUD.h"
#include "GUI/InventoryWindow.h"
#include "GUI/ShopWindow.h"
#include "GUI/PosShopWindow.h"
#include "GUI/RepairWindow.h"
#include "GUI/StashWindow.h"
#include "Gui/GuildStashWindow.h"
#include "GUI/Common/Item.h"

#include "Network/GameProtocol.h"
#include "Network/GamePacket.h"
#include "Network/GameSocket.h"

#include "Components/AttributeComponent.h"
#include "Components/PositionComponent.h"
#include "Components/InventoryComponent.h"

#include "Framework/GUI.h"

#include "Systems/MovementSystem.h"

#include <boost/format.hpp>
#include <iostream>

#include "Game.h"
#include "MagicData.h"
#include "SpecialAttackData.h"
#include "AbilityData.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

String HUD::EventQuickSlotChanged = "EventQuickSlotChanged";

HUD::HUD() :
quickSlotUpdating_(false),
inventoryFromIndex(-1),
quickToIndex(-1),
selectedMagicSlot_(nullptr)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("hud.layout");
		window_->activate();
		window_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&HUD::HandleKeyPress, this));
		window_->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&HUD::HandleWindowUpdated, this));
		System::getSingleton().setGUISheet(window_);

		AnimationManager::getSingletonPtr()->loadAnimationsFromXML("hud.animation");

		frame_ = window_->getChild("Hud/Frame");
		frame_->addEvent(EventQuickSlotChanged);
		frame_->subscribeEvent(EventQuickSlotChanged, Event::Subscriber(&HUD::HandleQuickSlotChanged, this));
		frame_->hide();

		outsideFrame_ = window_->getChild("Hud/OutsideFrame");
		outsideFrame_->hide();

		buttonMenu_	= outsideFrame_->getChild("Hud/ButtonMenuArea");
		buttonMenu_->hide();

		magicExtraSlot_	= outsideFrame_->getChild("Hud/MagicExtraSlot");
		magicExtraSlot_->hide();

		level_			= frame_->getChild("Hud/Level");
		xposition_		= frame_->getChild("Hud/XPosition");
		yposition_		= frame_->getChild("Hud/YPosition");
		moral_			= frame_->getChild("Hud/Moral");

		for (int i = 0; i < QUICK_SLOT_COUNT; ++i)
		{
			quickSlots_[i] = frame_->getChild("Hud/QuickSlot" + PropertyHelper::intToString(i+1));
			quickSlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&HUD::HandleQuickSlotDragDropItemDropped, this));
			quickSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&HUD::HandleMouseEnterQuickSlotArea, this));
			quickSlots_[i]->subscribeEvent(Window::EventMouseLeavesArea,
				Event::Subscriber(&HUD::HandleMouseLeaveQuickSlotArea, this));
			quickSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&HUD::HandleQuickSlotMouseClick, this));
			quickSlots_[i]->subscribeEvent(Window::EventMouseDoubleClick,
				Event::Subscriber(&HUD::HandleQuickSlotMouseDoubleClick, this));
		}

		magicSlotSelection_ = frame_->getChild("Hud/MagicSlotSelection");
		frame_->removeChildWindow(magicSlotSelection_);

		for (int i = 0; i < MAGIC_QUICK_SLOT_COUNT; ++i)
		{
			magicSlots_[i] = frame_->getChild("Hud/MagicSlot" + PropertyHelper::intToString(i+1));
		}

		for (int i = MAGIC_QUICK_SLOT_COUNT; i < MAGIC_QUICK_SLOT_COUNT + MAGIC_QUICK_SLOT_EXTRA_COUNT; ++i)
		{
			magicSlots_[i] = magicExtraSlot_->getChild("Hud/MagicSlot" + PropertyHelper::intToString(i+1));
		}

		for (int i = 0; i < MAGIC_SLOT_COUNT; ++i)
		{
			magicSlots_[i]->setID(i);
			magicSlots_[i]->setUserString("Type", "");
			magicSlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&HUD::HandleMagicSlotDragDropItemDropped, this));
			magicSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&HUD::HandleMouseEnterMagicSlotArea, this));
			magicSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&HUD::HandleMagicSlotMouseClick, this));
		}

		magicExtraSlotButton_ = static_cast<PushButton*>(frame_->getChild("Hud/MagicExtraSlotToggleButton"));
		magicExtraSlotButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUD::HandleMagicExtraSlotButton, this));

		runToggle_ = static_cast<Checkbox*>(frame_->getChild("Hud/RunToggle"));
		runToggle_->subscribeEvent(Checkbox::EventCheckStateChanged, Event::Subscriber(&HUD::HandleRunToggle, this));

		showMenuButton_ = static_cast<PushButton*>(outsideFrame_->getChild("Hud/ToggleMenuButton"));
		showMenuButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUD::HandleShowMenuButton, this));

		Window* menu = buttonMenu_->getChild("Hud/Menu");
	
		hideMenuButton_ = static_cast<PushButton*>(menu->getChild("Hud/Menu/HideButton"));
		hideMenuButton_->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&HUD::HandleHideMenuButton, this));

		menuBattleModeButton_ = static_cast<PushButton*>(menu->getChild("Hud/Menu/BattleModeButton"));
		menuBattleModeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUD::HandleBattleModeChange, this));

		menuStatusButton_ = static_cast<PushButton*>(menu->getChild("Hud/Menu/StatusButton"));
		menuStatusButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUD::HandleMenuStatusButton, this));

		menuInventoryButton_ = static_cast<PushButton*>(menu->getChild("Hud/Menu/InventoryButton"));
		menuInventoryButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUD::HandleMenuInventoryButton, this));

		menuChatButton_ = static_cast<PushButton*>(menu->getChild("Hud/Menu/ChatButton"));
		menuChatButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUD::HandleMenuChatButton, this));

		menuMagicButton_ = static_cast<PushButton*>(menu->getChild("Hud/Menu/MagicButton"));
		menuMagicButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUD::HandleMenuMagicButton, this));

		menuOptionButton_ = static_cast<PushButton*>(menu->getChild("Hud/Menu/OptionButton"));

		menuHelpButton_ = static_cast<PushButton*>(menu->getChild("Hud/Menu/HelpButton"));

		healthBar_ = static_cast<ProgressBar*>(frame_->getChild("Hud/HealthBar"));
		manaBar_ = static_cast<ProgressBar*>(frame_->getChild("Hud/ManaBar"));
		staminaBar_ = static_cast<ProgressBar*>(frame_->getChild("Hud/StaminaBar"));
		weightBar_ = static_cast<ProgressBar*>(frame_->getChild("Hud/WeightBar"));
		experienceBar_ = static_cast<ProgressBar*>(frame_->getChild("Hud/ExperienceBar"));

		battleModeIndicator_ = frame_->getChild("Hud/BattleModeIndicator");

		itemDragItem_ = createItemDragItem("Hud/ItemDragItem");

		itemDragContainer_ = createItemDragContainer("Hud/ItemDragContainer", itemDragItem_);
		itemDragContainer_->subscribeEvent(DragContainer::EventDragStarted, Event::Subscriber(&HUD::HandleItemDragStarted, this));
		itemDragContainer_->subscribeEvent(DragContainer::EventDragEnded, Event::Subscriber(&HUD::HandleItemDragEnded, this));
		itemDragContainer_->subscribeEvent(DragContainer::EventMouseButtonUp, Event::Subscriber(&HUD::HandleItemDragMouseButtonUp, this));

		magicDragItem_ = WindowManager::getSingleton().createWindow("Soma/StaticImage", "Hud/MagicDragItem");
		magicDragItem_->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		magicDragItem_->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
		magicDragItem_->disable();

		magicDragContainer_ = static_cast<DragContainer*>(
				WindowManager::getSingleton().createWindow("DragContainer", "Hud/MagicDragContainer"));
		magicDragContainer_->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		magicDragContainer_->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
		magicDragContainer_->addChildWindow(magicDragItem_);
		magicDragContainer_->setDragAlpha(1.0f);
		magicDragContainer_->setProperty("MouseInputPropagationEnabled", "True");
		magicDragContainer_->setDragDropTarget(false);

		magicDragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&HUD::HandleMagicDragStarted, this));

		magicDragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&HUD::HandleMagicDragEnded, this));

		magicDragContainer_->subscribeEvent(DragContainer::EventMouseButtonUp,
			Event::Subscriber(&HUD::HandleMagicDragMouseButtonUp, this));

		//Window* scrollingText = _window->getChild("Hud/ScrollingText");

		//Window* scrollingText2 = _window->getChild("Hud/ScrollingText2");

		//_scrollingText1 = 
		//	AnimationManager::getSingletonPtr()->instantiateAnimation("ScrollingText");
		//_scrollingText1->setTargetWindow(scrollingText);

		//_scrollingText2 = 
		//	AnimationManager::getSingletonPtr()->instantiateAnimation("ScrollingText");
		//_scrollingText2->setTargetWindow(scrollingText2);

		//_scrollingText1->start();
	}
	catch (Exception& e)
	{
		std::cerr << "HUD: Failed to initialize: " << e.what() << std::endl;
	}
}

HUD::~HUD()
{
	AnimationManager::getSingletonPtr()->destroyAnimation("ButtonMenuSlideDown");
	AnimationManager::getSingletonPtr()->destroyAnimation("ButtonMenuSlideUp");
	AnimationManager::getSingletonPtr()->destroyAnimation("ScrollingText");

	itemDragItem_->destroy();
	itemDragContainer_->destroy();
	magicDragItem_->destroy();
	magicDragContainer_->destroy();
	magicSlotSelection_->destroy();
	window_->destroy();
}

bool HUD::HandleHideMenuButton(const EventArgs& e)
{
	hideMenuButton_->disable();

	AnimationInstance * instance = AnimationManager::getSingletonPtr()->instantiateAnimation("ButtonMenuSlideDown");
	instance->setTargetWindow(buttonMenu_);
	instance->start();

	buttonMenu_->subscribeEvent(AnimationInstance::EventAnimationEnded,
		Event::Subscriber(&HUD::HandleHideMenuButtonAnimationEnded, this));
	return true;
}

bool HUD::HandleShowMenuButton(const EventArgs& e)
{
	showMenuButton_->disable();
	showMenuButton_->hide();
	buttonMenu_->show();
	hideMenuButton_->enable();

	buttonMenu_->removeEvent(AnimationInstance::EventAnimationEnded);

	AnimationInstance * instance = AnimationManager::getSingletonPtr()->instantiateAnimation("ButtonMenuSlideUp");
	instance->setTargetWindow(buttonMenu_);
	instance->start();
	return true;
}

bool HUD::HandleHideMenuButtonAnimationEnded(const EventArgs& e)
{
	buttonMenu_->hide();
	showMenuButton_->enable();
	showMenuButton_->show();
	return true;
}

bool HUD::HandleMagicExtraSlotButton(const EventArgs& e)
{
	magicExtraSlot_->setVisible(!magicExtraSlot_->isVisible());
	return true;
}

bool HUD::HandleKeyPress(const EventArgs& e)
{
	if (!frame_->isVisible() || !outsideFrame_->isVisible())
	{
		return false;
	}

	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent != nullptr && !attributeComponent->IsAlive())
	{
		return false;
	}

	const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);

	// TODO: Keep list of windows to close when other windows are visible. E.g. open inventory window will close other open windows.
	switch (keyArgs.scancode)
	{
	case Key::F1: // Fall-through!
	case Key::F2:
	case Key::F3:
	case Key::F4:
		SendUseItem(static_cast<unsigned int>(keyArgs.scancode - Key::F1));
		break;
	case Key::F5: // Fall-through!
	case Key::F6:
	case Key::F7:
	case Key::F8:
		{
		MouseEventArgs ma(magicSlots_[static_cast<unsigned int>(keyArgs.scancode - Key::F5)]);
		ma.button = MouseButton::LeftButton;
		magicSlots_[static_cast<unsigned int>(keyArgs.scancode - Key::F5)]->fireEvent(Window::EventMouseClick, ma);
		}
		break;
	case Key::F9:
		menuStatusButton_->fireEvent(PushButton::EventClicked, EventArgs());
		return true;
	case Key::F10:
		menuInventoryButton_->fireEvent(PushButton::EventClicked, EventArgs());
		return true;
	case Key::F11:
		menuMagicButton_->fireEvent(PushButton::EventClicked, EventArgs());
		return true;
	case Key::F12:
		menuChatButton_->fireEvent(PushButton::EventClicked, EventArgs());
		return true;
	case Key::Return:
		window_->getChild("ChatInput/Frame")->show();
		return true;
	case Key::Tab:
		menuBattleModeButton_->fireEvent(PushButton::EventClicked, EventArgs());
		return true;
	case Key::M:
		if (keyArgs.sysKeys == Control)
		{
			Window* mapFrame = window_->getChild("Map/Frame");
			mapFrame->setVisible(!mapFrame->isVisible());
			mapFrame->moveInFront(window_->getChild("FocusRoot"));
			mapFrame->activate();
			return true;
		}
		return false;
	case Key::J:
		if (keyArgs.sysKeys == Control)
		{
			Window* frame = window_->getChild("Quest/Frame");
			frame->setVisible(!frame->isVisible());
			frame->moveInFront(window_->getChild("FocusRoot"));
			frame->activate();
			return true;
		}
		return false;
	case Key::LeftControl:
	case Key::RightControl:
		runToggle_->setSelected(!runToggle_->isSelected());
		return true;
	}
	return false;
}

void HUD::Update()
{
	if (!frame_->isVisible() || !outsideFrame_->isVisible())
	{
		return;
	}

	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	PositionComponent* positionComponent = Game::Instance().myEntity->getComponent<PositionComponent>();
	if (attributeComponent == nullptr || positionComponent == nullptr)
	{
		return;
	}

	healthBar_->setProgress(attributeComponent->GetHealthStatus());
	healthBar_->setTooltipText(boost::str(boost::format("HP : %1%/%2%") %
		attributeComponent->GetHealth() % attributeComponent->GetMaximumHealth()));

	manaBar_->setProgress(attributeComponent->GetManaStatus());
	manaBar_->setTooltipText(boost::str(boost::format("MP : %1%/%2%")
		% attributeComponent->GetMana() % attributeComponent->GetMaximumMana()));

	staminaBar_->setProgress(attributeComponent->GetStaminaStatus());
	staminaBar_->setTooltipText(boost::str(boost::format("Stamina : %1%/%2%")
		% attributeComponent->GetStamina() % attributeComponent->GetMaximumStamina()));

	weightBar_->setProgress(attributeComponent->GetWeightStatus());
	weightBar_->setTooltipText(boost::str(boost::format("Weight : %1%/%2%")
		% attributeComponent->GetWeight() % attributeComponent->GetMaximumWeight()));

	experienceBar_->setProgress(attributeComponent->GetExpStatus());
	experienceBar_->setTooltipText(boost::str(boost::format("Experience : %1$.2f%%")
		% (attributeComponent->GetExpStatus() * 100)));

	level_->setText(PropertyHelper::intToString(attributeComponent->GetLevel()));

	xposition_->setText(boost::str(boost::format("X: %1%") % positionComponent->GetCellPosition().x));
	yposition_->setText(boost::str(boost::format("Y: %1%") % positionComponent->GetCellPosition().y));

	switch (attributeComponent->GetMoral())
	{
	case MoralType::Devil:
		moral_->setProperty("Image", "set:hud image:DevilMoral");
		break;
	case MoralType::Wicked:
		moral_->setProperty("Image", "set:hud image:WickedMoral");
		break;
	case MoralType::Evil:
		moral_->setProperty("Image", "set:hud image:EvilMoral");
		break;
	case MoralType::Bad:
		moral_->setProperty("Image", "set:hud image:BadMoral");
		break;
	case MoralType::Neutral:
		moral_->setProperty("Image", "set:hud image:NeutralMoral");
		break;
	case MoralType::Good:
		moral_->setProperty("Image", "set:hud image:GoodMoral");
		break;
	case MoralType::Moral:
		moral_->setProperty("Image", "set:hud image:MoralMoral");
		break;
	case MoralType::Virtuous:
		moral_->setProperty("Image", "set:hud image:VirtuousMoral");
		break;
	case MoralType::Angel:
		moral_->setProperty("Image", "set:hud image:AngelMoral");
		break;
	}
	moralValue_.SetValue(attributeComponent->GetMoralValue());
	moral_->setTooltipText(moralValue_.GetText());

	switch (attributeComponent->GetBattleMode())
	{
	case BattleMode::Normal:
		battleModeIndicator_->setProperty("Image", "set:hud image:BattleModeIndicatorPeace");
		menuBattleModeButton_->setProperty("NormalImage", "");
		break;
	case BattleMode::Battle:
		battleModeIndicator_->setProperty("Image", "set:hud image:BattleModeIndicatorBattle");
		menuBattleModeButton_->setProperty("NormalImage", "set:hud image:BattleModeButtonBattle");
		break;
	case BattleMode::PK:
		battleModeIndicator_->setProperty("Image", "set:hud image:BattleModeIndicatorPK");
		menuBattleModeButton_->setProperty("NormalImage", "set:hud image:BattleModeButtonPK");
		break;
	}

	// Here handling the case where the run toggle has not been altered not by the GUI.
	// Do not want it to fire the selected event which sends the run toggle packet
	// because it will have been sent already (if the other code does so which it should!).
	if (runToggle_->isSelected() != attributeComponent->GetRunToggle())
	{
		runToggle_->setMutedState(true);
		runToggle_->setSelected(attributeComponent->GetRunToggle());
		runToggle_->setMutedState(false);
	}

	//if (!_scrollingText2->isRunning())
	//{
	//	if (_scrollingText1->getPosition() >= (_scrollingText1->getDefinition()->getDuration() / 2.f))
	//	{
	//		_scrollingText2->start();
	//	}
	//}
}

bool HUD::HandleWindowUpdated(const EventArgs& e)
{
	Update();
	return true;
}

bool HUD::HandleMenuStatusButton(const EventArgs& e)
{
	Window* statusFrame = window_->getChild("Status/Frame");
	statusFrame->setVisible(!statusFrame->isVisible());
	statusFrame->moveInFront(window_->getChild("FocusRoot"));
	statusFrame->activate();

	Window* chatHistoryFrame = window_->getChild("ChatHistory/Frame");
	chatHistoryFrame->hide();

	Window* inventoryFrame = window_->getChild("Inventory/Frame");
	inventoryFrame->hide();

	Window* magicAuraFrame = window_->getChild("MagicAura/Frame");
	magicAuraFrame->hide();
	return true;
}

bool HUD::HandleMenuInventoryButton(const EventArgs& e)
{
	Window* inventoryFrame = window_->getChild("Inventory/Frame");
	inventoryFrame->setVisible(!inventoryFrame->isVisible());
	inventoryFrame->moveInFront(window_->getChild("FocusRoot"));
	inventoryFrame->activate();

	Window* chatHistoryFrame = window_->getChild("ChatHistory/Frame");
	chatHistoryFrame->hide();

	Window* statusFrame = window_->getChild("Status/Frame");
	statusFrame->hide();

	Window* magicAuraFrame = window_->getChild("MagicAura/Frame");
	magicAuraFrame->hide();
	return true;
}

bool HUD::HandleMenuChatButton(const EventArgs& e)
{
	Window* chatHistoryFrame = window_->getChild("ChatHistory/Frame");
	chatHistoryFrame->setVisible(!chatHistoryFrame->isVisible());
	chatHistoryFrame->moveInFront(window_->getChild("FocusRoot"));
	chatHistoryFrame->activate();

	Window* statusFrame = window_->getChild("Status/Frame");
	statusFrame->hide();

	Window* inventoryFrame = window_->getChild("Inventory/Frame");
	inventoryFrame->hide();

	Window* magicAuraFrame = window_->getChild("MagicAura/Frame");
	magicAuraFrame->hide();
	return true;
}

bool HUD::HandleMenuMagicButton(const EventArgs& e)
{
	Window* magicAuraFrame =window_->getChild("MagicAura/Frame");
	magicAuraFrame->setVisible(!magicAuraFrame->isVisible());
	magicAuraFrame->moveInFront(window_->getChild("FocusRoot"));
	magicAuraFrame->activate();

	Window* chatHistoryFrame = window_->getChild("ChatHistory/Frame");
	chatHistoryFrame->hide();

	Window* statusFrame = window_->getChild("Status/Frame");
	statusFrame->hide();

	Window* inventoryFrame = window_->getChild("Inventory/Frame");
	inventoryFrame->hide();
	return true;
}

bool HUD::HandleMenuOptionButton(const EventArgs& e)
{
	return true;
}

bool HUD::HandleMenuHelpButton(const EventArgs& e)
{
	return true;
}

bool HUD::HandleQuickSlotDragDropItemDropped(const EventArgs& e)
{
    const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	const ItemAttributeComponent* itemFrom = static_cast<ItemAttributeComponent*>(dragDropArgs.dragDropItem->getUserData());
	const ItemAttributeComponent* itemTo = static_cast<ItemAttributeComponent*>(dragDropArgs.window->getUserData());
	if (dragDropArgs.dragDropItem == itemDragContainer_)
	{
		if (itemFrom != nullptr && itemTo != nullptr && itemFrom->GetIndex() != itemTo->GetIndex())
		{
			SendChangeBeltIndex(itemFrom->GetIndex(), itemTo->GetIndex());
		}
		updateItemDragContainer(*dragDropArgs.window, *itemDragContainer_, *itemDragItem_);
	}
	else if (dragDropArgs.dragDropItem->getName().compare("Inventory/DragContainer") == 0)
	{
		if (itemFrom != nullptr && itemTo != nullptr && !itemFrom->Empty() &&
			itemFrom->GetType() == ItemType::Food || itemFrom->GetType() == ItemType::Potion)
		{
			SendItemToBelt(itemFrom->GetIndex(), itemTo->GetIndex());
		}
	}
	return true;
}

void HUD::SendItemToBelt(const unsigned short fromIndex, const unsigned short toIndex)
{
	if (!quickSlotUpdating_)
	{
		inventoryFromIndex = fromIndex;
		quickToIndex = toIndex;
		quickSlotUpdating_ = true;

		GamePacket packet(GameProtocol::PKT_PUTITEM_BELT);
		packet << fromIndex << toIndex;
		Game::Instance().gameSocket->Send(packet);
	}
}

void HUD::UpdateQuickSlots()
{
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (inventoryComponent == nullptr)
	{
		return;
	}

	for (int i = 0; i < QUICK_SLOT_COUNT; ++i)
	{
		inventoryComponent->GetBeltItem(i).SetIndex(i);
		setItemSlot(*quickSlots_[i], &inventoryComponent->GetBeltItem(i));
	}
}

bool HUD::HandleItemDragStarted(const EventArgs& e)
{
	if (quickSlotUpdating_)
	{
		itemDragContainer_->setDraggingEnabled(false);
		return true;
	}
	return handleItemDragStarted(*itemDragContainer_, *itemDragItem_, e);
}

bool HUD::HandleItemDragEnded(const EventArgs& e)
{
	return handleItemDragEnded(*itemDragItem_, e);
}

bool HUD::HandleItemDragMouseButtonUp(const EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		Window* inventoryFrame = window_->getChild("Inventory/Frame");
		if ((!inventoryFrame->isVisible() || !inventoryFrame->isHit(mouseEventArgs.position)) &&
			!frame_->isHit(mouseEventArgs.position))
		{
			const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(mouseEventArgs.window->getUserData());
			if (item != nullptr && !item->Empty())
			{
				itemDragItem_->setAlpha(0.0f);
				SendThrowItem(item->GetIndex(), 1);
			}
		}
	}
	return true;
}

bool HUD::HandleMouseEnterQuickSlotArea(const EventArgs& e)
{
	return handleMouseEnterItemSlotArea(itemDragContainer_, itemDragItem_, e);
}

bool HUD::HandleMouseLeaveQuickSlotArea(const EventArgs& e)
{
	return handleMouseLeaveItemSlotArea(e);
}

void HUD::HandlePutItemBelt(GamePacket& packetRecv)
{
	sf::Uint8 result;
	packetRecv >> result;

	if (result == 1 || result == 3)
	{
		InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
		if (inventoryComponent != nullptr)
		{
			if (result == 1)
			{
				std::swap(inventoryComponent->GetInventoryItem(inventoryFromIndex),
						  inventoryComponent->GetBeltItem(quickToIndex));
			}
			else if (result == 3)
			{
				inventoryComponent->GetInventoryItem(inventoryFromIndex).SetDura(0);
			}

			Game::Instance().gui->FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
			Game::Instance().gui->FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);
			UpdateQuickSlots();
		}
	}
	quickSlotUpdating_ = false;
}

void HUD::SendChangeBeltIndex(const unsigned short fromIndex, const unsigned short toIndex)
{
	GamePacket packet(GameProtocol::PKT_CHANGEBELTINDEX);
	packet << fromIndex << toIndex;
	Game::Instance().gameSocket->Send(packet);
}

bool HUD::HandleQuickSlotMouseClick(const EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == RightButton)
	{
		const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(mouseEventArgs.window->getUserData());
		if (item != nullptr && !item->Empty())
		{
			if (item->GetEquipPosition() == ItemEquipPosition::Use)
			{
				SendUseItem(item->GetIndex());
			}
		}
	}
	return true;
}

bool HUD::HandleQuickSlotMouseDoubleClick(const EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(mouseEventArgs.window->getUserData());
		if (item != nullptr && !item->Empty())
		{
			if (item->GetEquipPosition() == ItemEquipPosition::Use)
			{
				SendUseItem(item->GetIndex());
			}
		}
	}
	return true;
}

void HUD::SendUseItem(const unsigned short index)
{
	GamePacket packet(GameProtocol::PKT_ITEM_USE);
	packet << static_cast<sf::Uint8>(0) << index; // TODO: BELT_INV, need types!
	Game::Instance().gameSocket->Send(packet);
}

void HUD::SendThrowItem(const unsigned short index, const unsigned short count)
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	PositionComponent* positionComponent = Game::Instance().myEntity->getComponent<PositionComponent>();
	if (attributeComponent != nullptr && positionComponent != nullptr )
	{
		GamePacket packet(GameProtocol::PKT_ITEM_THROW);
		packet << static_cast<sf::Uint8>(0)
			   << index
			   << count
			   << static_cast<sf::Int16>(positionComponent->GetCellPosition().x)
			   << static_cast<sf::Int16>(positionComponent->GetCellPosition().y);
		Game::Instance().gameSocket->Send(packet);
	}
}

void HUD::UpdateMagicDragContainer(Window& window)
{
	// The item being dragged and the drag container is shared for all the slots
	// therefore it needs to be placed as a child window for the slot the mouse is
	// currently within.

	// The alpha is set to 0 to avoid the image of the drag item
	// being seen while not being dragged.
	magicDragContainer_->setID(window.getID());
	magicDragContainer_->setUserData(window.getUserData());
	magicDragContainer_->setUserString("Type", window.getUserString("Type"));
	window.addChildWindow(magicDragContainer_);
}

bool HUD::HandleMagicDragStarted(const EventArgs& e)
{
	if (magicDragContainer_->getUserData())
	{
		String type = magicDragContainer_->getUserString("Type");
		if (type.compare("Magic") == 0)
		{
			const MagicData& magic = *static_cast<MagicData*>(magicDragContainer_->getUserData());
			magicDragItem_->setProperty("Image", "set:magiciconsbig image:" + PropertyHelper::intToString(magic.id_));
			magicDragItem_->setAlpha(1.0f);
		}
		else if (type.compare("SpecialAttack") == 0)
		{
			const SpecialAttackData& specialAttack = *static_cast<SpecialAttackData*>(magicDragContainer_->getUserData());
			magicDragItem_->setProperty("Image", "set:specialattackiconsbig image:" + PropertyHelper::intToString(specialAttack.id_));
			magicDragItem_->setAlpha(1.0f);
		}
		else if (type.compare("Ability") == 0)
		{
			const AbilityData& ability = *static_cast<AbilityData*>(magicDragContainer_->getUserData());
			magicDragItem_->setProperty("Image", "set:abilityiconsbig image:" + PropertyHelper::intToString(ability.id_));
			magicDragItem_->setAlpha(1.0f);
		}
	}

	return true;
}

bool HUD::HandleMagicDragEnded(const EventArgs& e)
{
	magicDragItem_->setAlpha(0.0f);
	return true;
}

bool HUD::HandleMouseEnterMagicSlotArea(const EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.window->getUserData())
	{
		// Must not update the drag item if a item is currently
		// being dragged.
		if (!magicDragContainer_->isBeingDragged())
		{
			UpdateMagicDragContainer(*mouseEventArgs.window);
		}
	}
	return true;
}

bool HUD::HandleMagicSlotDragDropItemDropped(const EventArgs& e)
{
    const DragDropEventArgs& dragDropArgs = static_cast<const DragDropEventArgs&>(e);
	if (dragDropArgs.dragDropItem->getName().compare("HUD/MagicDragContainer") == 0)
	{
		Window* slotFrom = magicSlots_[dragDropArgs.dragDropItem->getID()];
		Window* slotTo	= magicSlots_[dragDropArgs.window->getID()];

		String imageFrom = slotFrom->getProperty("Image");
		String imageTo	 = slotTo->getProperty("Image");
		slotFrom->setProperty("Image", imageTo);
		slotTo->setProperty("Image", imageFrom);

		String typeFrom = slotFrom->getUserString("Type");
		String typeTo   = slotTo->getUserString("Type");
		slotFrom->setUserString("Type", typeTo);
		slotTo->setUserString("Type", typeFrom);

		void* dataFrom = slotFrom->getUserData();
		void* dataTo   = slotTo->getUserData();
		slotFrom->setUserData(dataTo);
		slotTo->setUserData(dataFrom);

		String tooltipFrom = slotFrom->getTooltipText();
		String tooltipTo   = slotTo->getTooltipText();
		slotFrom->setTooltipText(tooltipTo);
		slotTo->setTooltipText(tooltipFrom);

		UpdateMagicDragContainer(*dragDropArgs.window);
	}
	else if (dragDropArgs.dragDropItem->isUserStringDefined("Type"))
	{
		if (dragDropArgs.dragDropItem->getUserString("Type").compare("SpecialAttack") == 0)
		{
			if (dragDropArgs.dragDropItem->getUserData())
			{
				const SpecialAttackData& specialAttack =
					*static_cast<SpecialAttackData*>(dragDropArgs.dragDropItem->getUserData());

				dragDropArgs.window->setProperty("Image",
					"set:specialattackiconssmall image:" + PropertyHelper::intToString(specialAttack.id_));
				dragDropArgs.window->setUserString("Type", "SpecialAttack");
				dragDropArgs.window->setUserData(dragDropArgs.dragDropItem->getUserData());
				dragDropArgs.window->setTooltipText(dragDropArgs.dragDropItem->getTooltipText());
			}

			UpdateMagicDragContainer(*dragDropArgs.window);
		}
		else if (dragDropArgs.dragDropItem->getUserString("Type").compare("Magic") == 0)
		{
			if (dragDropArgs.dragDropItem->getUserData())
			{
				const MagicData& magic =
					*static_cast<MagicData*>(dragDropArgs.dragDropItem->getUserData());

				dragDropArgs.window->setProperty("Image",
					"set:magiciconssmall image:" + PropertyHelper::intToString(magic.id_));
				dragDropArgs.window->setUserString("Type", "Magic");
				dragDropArgs.window->setUserData(dragDropArgs.dragDropItem->getUserData());
				dragDropArgs.window->setTooltipText(dragDropArgs.dragDropItem->getTooltipText());
			}

			UpdateMagicDragContainer(*dragDropArgs.window);
		}
		else if (dragDropArgs.dragDropItem->getUserString("Type").compare("Ability") == 0)
		{
			if (dragDropArgs.dragDropItem->getUserData())
			{
				const AbilityData& ability =
					*static_cast<AbilityData*>(dragDropArgs.dragDropItem->getUserData());

				dragDropArgs.window->setProperty("Image",
					"set:abilityiconssmall image:" + PropertyHelper::intToString(ability.id_));
				dragDropArgs.window->setUserString("Type", "Ability");
				dragDropArgs.window->setUserData(dragDropArgs.dragDropItem->getUserData());
				dragDropArgs.window->setTooltipText(dragDropArgs.dragDropItem->getTooltipText());
			}

			UpdateMagicDragContainer(*dragDropArgs.window);
		}
	}

	return true;
}

bool HUD::HandleMagicDragMouseButtonUp(const EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		if (!frame_->isHit(mouseEventArgs.position) &&
			(!magicExtraSlot_->isVisible() || !magicExtraSlot_->isHit(mouseEventArgs.position)))
		{
			if (mouseEventArgs.window->getUserData() != nullptr)
			{
				Window* slot = magicSlots_[mouseEventArgs.window->getID()];
				slot->setProperty("Image", "");
				slot->setUserString("Type", "");
				slot->setUserData(nullptr);
				slot->setTooltipText("");
				if (selectedMagicSlot_ == slot)
				{
					selectedMagicSlot_ = nullptr;
				}
			}
		}
	}
	return true;
}

bool HUD::HandleMagicSlotMouseClick(const CEGUI::EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton && mouseEventArgs.window->getUserData() != nullptr)
	{
		if (!mouseEventArgs.window->isChild(magicSlotSelection_))
		{
			mouseEventArgs.window->addChildWindow(magicSlotSelection_);
			selectedMagicSlot_ = mouseEventArgs.window;
		}
		else
		{
			mouseEventArgs.window->removeChildWindow(magicSlotSelection_);
			selectedMagicSlot_ = nullptr;
		}
	}
	return true;
}

void HUD::HandleSetMagicSlots(GamePacket& packetRecv)
{
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (!inventoryComponent)
	{
		return;
	}

	for (int i = 0; i < MAGIC_SLOT_COUNT; ++i)
	{
		sf::Uint8 type;
		packetRecv >> type;

		sf::Int16 id;
		packetRecv >> id;

		if (id != -1)
		{
			switch (type)
			{
			case 0:
				{
					SpecialAttackDataPtr specialAttack = inventoryComponent->GetSpecialAttackById(id);
					if (specialAttack)
					{
						magicSlots_[i]->setProperty("Image",
							"set:specialattackiconssmall image:" + PropertyHelper::intToString(specialAttack->id_));
						magicSlots_[i]->setUserString("Type", "SpecialAttack");
						magicSlots_[i]->setUserData(specialAttack.get());

						std::string healthText;
						if (specialAttack->healthCost_)
						{
							healthText = boost::str(boost::format("HP %1%") % specialAttack->healthCost_);
						}

						std::string staminaText;
						if (specialAttack->staminaCost_)
						{
							staminaText = boost::str(boost::format("Stam %1%") % specialAttack->staminaCost_);
						}
						magicSlots_[i]->setTooltipText(boost::str(boost::format("%1%(%2%)/ MP %3%%4%%5%")
							% specialAttack->name_ % specialAttack->WeaponTypeToString() % specialAttack->manaCost_
							% healthText % staminaText));
					}
				}
				break;
			case 1:
				{
					MagicDataPtr magic = inventoryComponent->GetMagic(id);
					if (magic)
					{
						magicSlots_[i]->setProperty("Image",
							"set:magiciconssmall image:" + PropertyHelper::intToString(magic->id_));
						magicSlots_[i]->setUserString("Type", "Magic");
						magicSlots_[i]->setUserData(magic.get());
						magicSlots_[i]->setTooltipText(boost::str(boost::format("%1%/%2%")
							% magic->name_ % magic->manaCost_));
					}
				}
				break;
			case 2:
				{
					AbilityDataPtr ability = inventoryComponent->GetAbility(id);
					if (ability)
					{
						magicSlots_[i]->setProperty("Image",
							"set:magiciconssmall image:" + PropertyHelper::intToString(ability->id_));
						magicSlots_[i]->setUserString("Type", "Ability");
						magicSlots_[i]->setUserData(ability.get());
						magicSlots_[i]->setTooltipText(boost::str(boost::format("%1% %2%/%3%")
							% ability->name_ % ability->useCount_ % ability->maximumUseCount_));
					}
				}
				break;
			}
		}
		else
		{
			magicSlots_[i]->setProperty("Image", "");
			magicSlots_[i]->setUserString("Type", "");
			magicSlots_[i]->setUserData(nullptr);
			magicSlots_[i]->setTooltipText("");

		}
	}
}

bool HUD::HandleQuickSlotChanged(const EventArgs& e)
{
	UpdateQuickSlots();
	return true;
}

bool HUD::HandleRunToggle(const EventArgs& e)
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent != nullptr)
	{
		const bool selected = runToggle_->isSelected();
		if (!selected || (attributeComponent->GetStamina() > 0 && selected))
		{
			attributeComponent->SetRunToggle(selected);
			MovementSystem::SendPlayerRunToggle(selected);
		}
	}
	return true;
}

bool HUD::HandleBattleModeChange(const EventArgs& e)
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent != nullptr)
	{
		BattleMode::Enum battleMode = attributeComponent->GetBattleMode();
		if (battleMode == BattleMode::Normal)
		{
			battleMode = BattleMode::Battle;
		}
		else if (battleMode == BattleMode::Battle)
		{
			battleMode = BattleMode::PK;
		}
		else
		{
			battleMode = BattleMode::Normal;
		}

		Network::GamePacket packet(Network::GameProtocol::PKT_BATTLEMODE);
		packet << static_cast<unsigned char>(battleMode);
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

void HUD::Show()
{
	frame_->show();
	outsideFrame_->show();
	Update();
}

void HUD::Hide()
{
	frame_->hide();
	outsideFrame_->hide();
}

unsigned short HUD::GetSelectedMagicId()
{
	if (selectedMagicSlot_ == nullptr)
	{
		return 0;
	}

	if (!selectedMagicSlot_->isUserStringDefined("Type") || selectedMagicSlot_->getUserString("Type").compare("Magic") != 0)
	{
		return 0;
	}

	const MagicData& magic = *static_cast<MagicData*>(selectedMagicSlot_->getUserData());
	return magic.id_;
}

} // namespace GUI