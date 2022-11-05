#ifndef GUI_HUD_H
#define GUI_HUD_H

#include <CEGUI/CEGUI.h>
#include <boost/scoped_ptr.hpp>
#include "GUI/Moral.h"

class ItemAttributeComponent;

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class ChatInput;
class ChatHistoryWindow;

const int QUICK_SLOT_COUNT = 4;
const int MAGIC_QUICK_SLOT_COUNT = 4;
const int MAGIC_QUICK_SLOT_EXTRA_COUNT = 8;
const int MAGIC_SLOT_COUNT = 12;

class HUD
{
public:
	static CEGUI::String EventQuickSlotChanged;

	HUD();
	~HUD();

	void HandlePutItemBelt(Network::GamePacket& packetRecv);

	void HandleSetMagicSlots(Network::GamePacket& packetRecv);

	void Show();
	void Hide();

	unsigned short GetSelectedMagicId();

private:
	void UpdateQuickSlots();

	void Update();

	bool HandleHideMenuButton(const CEGUI::EventArgs& e);
	bool HandleShowMenuButton(const CEGUI::EventArgs& e);
	bool HandleHideMenuButtonAnimationEnded(const CEGUI::EventArgs& e);
	bool HandleMagicExtraSlotButton(const CEGUI::EventArgs& e);

	bool HandleKeyPress(const CEGUI::EventArgs& e);
	bool HandleWindowUpdated(const CEGUI::EventArgs& e);

	bool HandleMenuStatusButton(const CEGUI::EventArgs& e);
	bool HandleMenuInventoryButton(const CEGUI::EventArgs& e);
	bool HandleMenuChatButton(const CEGUI::EventArgs& e);
	bool HandleMenuMagicButton(const CEGUI::EventArgs& e);
	bool HandleMenuOptionButton(const CEGUI::EventArgs& e);
	bool HandleMenuHelpButton(const CEGUI::EventArgs& e);

	bool HandleQuickSlotDragDropItemDropped(const CEGUI::EventArgs& e);
	bool HandleItemDragStarted(const CEGUI::EventArgs& e);
	bool HandleItemDragEnded(const CEGUI::EventArgs& e);
	bool HandleItemDragMouseButtonUp(const CEGUI::EventArgs& e);
	bool HandleMouseEnterQuickSlotArea(const CEGUI::EventArgs& e);
	bool HandleMouseLeaveQuickSlotArea(const CEGUI::EventArgs& e);
	bool HandleQuickSlotMouseClick(const CEGUI::EventArgs& e);
	bool HandleQuickSlotMouseDoubleClick(const CEGUI::EventArgs& e);

	void SendItemToBelt(unsigned short fromIndex, unsigned short toIndex);

	void SendChangeBeltIndex(unsigned short fromIndex, unsigned short toIndex);
	void SendUseItem(unsigned short index);
	void SendThrowItem(unsigned short index, unsigned short count);

	void UpdateMagicDragContainer(CEGUI::Window& window);

	bool HandleMagicDragStarted(const CEGUI::EventArgs& e);
	bool HandleMagicDragEnded(const CEGUI::EventArgs& e);
	bool HandleMouseEnterMagicSlotArea(const CEGUI::EventArgs& e);
	bool HandleMagicSlotDragDropItemDropped(const CEGUI::EventArgs& e);
	bool HandleMagicDragMouseButtonUp(const CEGUI::EventArgs& e);
	bool HandleMagicSlotMouseClick(const CEGUI::EventArgs& e);

	bool HandleQuickSlotChanged(const CEGUI::EventArgs& e);

	bool HandleRunToggle(const CEGUI::EventArgs& e);

	bool HandleBattleModeChange(const CEGUI::EventArgs& e);

public:
	short inventoryFromIndex; // The inventory slot index from where the item being moved to belt is from.
	short quickToIndex;		  // The quick slot index to where the item is being moved from inventory is going to.

private:
	CEGUI::Window*		window_;
	CEGUI::Window*		frame_;
	CEGUI::Window*      outsideFrame_;
	CEGUI::Window*		buttonMenu_;
	CEGUI::PushButton*	showMenuButton_;
	CEGUI::PushButton*	hideMenuButton_;
	CEGUI::Window*		magicExtraSlot_;
	CEGUI::PushButton*	magicExtraSlotButton_;
	CEGUI::ProgressBar*	healthBar_;
	CEGUI::ProgressBar*	manaBar_;
	CEGUI::ProgressBar*	staminaBar_;
	CEGUI::ProgressBar*	weightBar_;
	CEGUI::ProgressBar*	experienceBar_;
	CEGUI::Window*		level_;
	CEGUI::Window*		xposition_;
	CEGUI::Window*		yposition_;
	CEGUI::Window*		moral_;
	CEGUI::Window*		quickSlots_[QUICK_SLOT_COUNT];
	CEGUI::Window*      magicSlotSelection_;
	CEGUI::Window*		magicSlots_[MAGIC_SLOT_COUNT];
	CEGUI::PushButton*	menuBattleModeButton_;
	CEGUI::PushButton*	menuStatusButton_;
	CEGUI::PushButton*	menuInventoryButton_;
	CEGUI::PushButton*	menuChatButton_;
	CEGUI::PushButton*	menuMagicButton_;
	CEGUI::PushButton*	menuOptionButton_;
	CEGUI::PushButton*	menuHelpButton_;
	CEGUI::Window*		battleModeIndicator_;
	CEGUI::Checkbox*	runToggle_;

	CEGUI::AnimationInstance* scrollingText1_;
	CEGUI::AnimationInstance* scrollingText2_;

	Moral moralValue_;

	CEGUI::DragContainer* itemDragContainer_;
	CEGUI::Window*		  itemDragItem_;
	bool				  quickSlotUpdating_;

	CEGUI::DragContainer* magicDragContainer_;
	CEGUI::Window*		  magicDragItem_;

	CEGUI::Window* selectedMagicSlot_;
};

} // namespace GUI

#endif // GUI_HUD_H