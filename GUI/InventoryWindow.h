#ifndef GUI_INVENTORY_WINDOW_H
#define GUI_INVENTORY_WINDOW_H

#include <CEGUI/CEGUI.h>
#include "GUI/CharacterRender.h"

namespace Network
{
	class GamePacket;
}

class ItemAttributeComponent;

namespace GUI
{

class InventoryWindow
{
public:
	static const int EQUIPMENT_SLOT_COUNT = 10;
	static const int INVENTORY_SLOT_COUNT = 10;

    static CEGUI::String EventInventoryChanged;

	InventoryWindow();
	~InventoryWindow();

	void HandlePutItemInv(Network::GamePacket& packetRecv);
	void HandleChangeItemIndex(Network::GamePacket& packetRecv);

private:
	bool HandleCharacterRenderUpdated(const CEGUI::EventArgs& e);
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleScrollInventory(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleMouseEnterSlotArea(const CEGUI::EventArgs& e);
	bool HandleMouseLeaveSlotArea(const CEGUI::EventArgs& e);
	bool HandleScrollbarPositionChanged(const CEGUI::EventArgs& e);
	bool HandleInventoryDragDropItemDropped(const CEGUI::EventArgs& e);
	bool HandleEquipmentDragDropItemDropped(const CEGUI::EventArgs& e);
	bool HandleCharacterDragDropItemDropped(const CEGUI::EventArgs& e);
	bool HandleDragStarted(const CEGUI::EventArgs& e);
	bool HandleDragEnded(const CEGUI::EventArgs& e);
	bool HandleDragMouseButtonUp(const CEGUI::EventArgs& e);
	bool HandleOpen(const CEGUI::EventArgs& e);
	bool HandleInventoryChanged(const CEGUI::EventArgs& e);
	bool HandleInventoryMouseClick(const CEGUI::EventArgs& e);
	bool HandleInventoryMouseDoubleClick(const CEGUI::EventArgs& e);
	bool HandleEquipmentMouseClick(const CEGUI::EventArgs& e);
	bool HandleEquipmentMouseDoubleClick(const CEGUI::EventArgs& e);
	bool HandleThrowMoney(const CEGUI::EventArgs& e);
	bool HandleThrowMoneyInputResult(const CEGUI::EventArgs& e);
	bool HandleThrowItemInputResult(const CEGUI::EventArgs& e);

	void SendChangeItem(short fromIndex, short toIndex = -1);
	void Update();
	void SendUseItem(unsigned short index);
	void SendItemToInv(unsigned short fromIndex, unsigned short toIndex);
	void SendThrowItem(unsigned short index, unsigned short count);
	void SendThrowMoney(unsigned int money);
	void StartEquipEffect(CEGUI::Window& equipmentSlot);
	void EquipOrUseItem(CEGUI::Window& window);
	void UnequipItem(CEGUI::Window& window);

private:
	short inventoryToIndex;	// The inventory slot index to where the item is being moved from belt is going to.
	short quickFromIndex;	// The quick slot index from where the item being moved to inventory is from.
	bool  inventorySlotUpdating;

	CEGUI::Window*		  window_;
	CEGUI::Window*		  characterRenderArea_;
	CEGUI::Window*		  characterRenderImage_;
	CEGUI::Window*		  equipmentSlots_[EQUIPMENT_SLOT_COUNT];
	CEGUI::Window*		  inventorySlots_[INVENTORY_SLOT_COUNT];
	CEGUI::Scrollbar*	  scrollBar_;
	CEGUI::PushButton*	  moneyButton_;
	CEGUI::Window*		  physicalDamage_;
	CEGUI::Window*		  physicalDefense_;
	CEGUI::Window*		  magicDamage_;
	CEGUI::Window*		  magicDefense_;
	CEGUI::Window*		  money_;
	CEGUI::PushButton*	  closeButton_;
	CEGUI::DragContainer* dragContainer_;
	CEGUI::Window*		  dragItem_;
	CEGUI::Window*		  blockedSlot_;
	CEGUI::Window*		  equipEffect_;
	CEGUI::AnimationInstance*
						  equipEffectAnimationInstance_;
	CharacterRender characterRender_;
};

} // namespace GUI

#endif // GUI_INVENTORY_WINDOW_H