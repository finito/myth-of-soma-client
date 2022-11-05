#ifndef GUI_REPAIR_WINDOW_H
#define GUI_REPAIR_WINDOW_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class RepairWindow
{
public:
	static CEGUI::String EventInventoryChanged;

	RepairWindow();
	~RepairWindow();

	void HandleRepairItem(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleMouseEnterSlotArea(const CEGUI::EventArgs& e);
	bool HandleMouseLeaveSlotArea(const CEGUI::EventArgs& e);
	bool HandleSlotMouseClickSingle(const CEGUI::EventArgs& e);
	bool HandleSlotMouseDoubleClick(const CEGUI::EventArgs& e);
	bool HandleScrollInventory(const CEGUI::EventArgs& e);
	bool HandleRepairButtonClicked(const CEGUI::EventArgs& e);
	bool HandleInventoryChanged(const CEGUI::EventArgs& e);
	bool HandleDragStarted(const CEGUI::EventArgs& e);
	bool HandleDragEnded(const CEGUI::EventArgs& e);
	bool HandleInventoryDragDropItemDropped(const CEGUI::EventArgs& e);
	bool HandleRepairDragDropItemDropped(const CEGUI::EventArgs& e);

	void Update();

	bool HandleSlotMouseClick(const CEGUI::EventArgs& e, bool doubleClick);

	void UpdateRepairSlot(CEGUI::Window& window);

	void SendRepairItem(short index);

private:
	static const int EQUIPMENT_SLOT_COUNT = 10; // Taken from InventoryWindow.h
	static const int INVENTORY_SLOT_COUNT = 10;

	CEGUI::Window*		  window_;
	CEGUI::PushButton*	  closeButton_;
	CEGUI::PushButton*	  repairButton_;
	CEGUI::Scrollbar*	  inventoryScrollBar_;
	CEGUI::Window*		  money_;
	CEGUI::Window*		  inventorySlots_[INVENTORY_SLOT_COUNT];
	CEGUI::Window*		  repairSlot_;
	bool				  processing_;
	CEGUI::DragContainer* dragContainer_;
	CEGUI::Window*		  dragItem_;

	typedef std::vector<ItemAttributeComponent*> ItemPtrArray;
	ItemPtrArray inventoryItems_;
};

} // namespace GUI

#endif // GUI_REPAIR_WINDOW_H