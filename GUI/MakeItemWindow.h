#ifndef GUI_MAKE_ITEM_WINDOW_H
#define GUI_MAKE_ITEM_WINDOW_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class MakeItemWindow
{
public:
	static CEGUI::String EventInventoryChanged;

	MakeItemWindow();
	~MakeItemWindow();

	void HandleMakeItemOpen(Network::GamePacket& packet);
	void HandleProduceItem(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleMouseEnterSlotArea(const CEGUI::EventArgs& e);
	bool HandleMouseLeaveSlotArea(const CEGUI::EventArgs& e);
	bool HandleScrollInventory(const CEGUI::EventArgs& e);
	bool HandleDragStarted(const CEGUI::EventArgs& e);
	bool HandleDragEnded(const CEGUI::EventArgs& e);
	bool HandleAddItemInputResult(const CEGUI::EventArgs& e);
	bool HandleMaterialSlotDragDropItemDropped(const CEGUI::EventArgs& e);
	bool HandleInventorySlotDragDropItemDropped(const CEGUI::EventArgs& e);
	bool HandleProduceButtonClicked(const CEGUI::EventArgs& e);
	bool HandleTypeButtonSelectStateChanged(const CEGUI::EventArgs& e);
	bool HandleItemNameInputResult(const CEGUI::EventArgs&e);
	bool HandleNameButtonClicked(const CEGUI::EventArgs& e);

	void Update();
	void UpdateItemData();

	void MoveItemToMaterialSlot(ItemAttributeComponent& item, const unsigned short count);
	void MoveItemToInventorySlot(const ItemAttributeComponent& item);

	void SendProduce();

private:
	static const int EQUIPMENT_SLOT_COUNT = 10; // Taken from InventoryWindow.h
	static const int INVENTORY_SLOT_COUNT = 10;

	static const int TYPE_SLOT_COUNT = 6; // The number of slots for the type buttons.
	static const int TYPE_COUNT = 23; // The number of buttons, one for each item type.
	static const int MATERIAL_ITEM_COUNT = 5;

	CEGUI::Window*		  window_;
	CEGUI::Window*        resultDescription_;
	CEGUI::Window*        resultSlot_;
	CEGUI::PushButton*	  okButton_;
	CEGUI::PushButton*    produceButton_;
	CEGUI::Checkbox*      genderCheckbox_;
	CEGUI::Window*        name_;
	CEGUI::PushButton*    nameButton_;
	CEGUI::Scrollbar*	  inventoryScrollBar_;
	CEGUI::Window*		  inventorySlots_[INVENTORY_SLOT_COUNT];
	CEGUI::Window*        typeSlots_[TYPE_SLOT_COUNT];
	CEGUI::RadioButton*   typeButtons_[TYPE_COUNT];
	CEGUI::Window*        materialSlots_[MATERIAL_ITEM_COUNT];
	CEGUI::Window*        materialSlotArea_;
	bool				  processing_;
	CEGUI::DragContainer* dragContainer_;
	CEGUI::Window*		  dragItem_;

	typedef std::vector<ItemAttributeComponent> ItemArray;
	ItemArray inventoryItems_;
	ItemArray materialItems_;
};

} // namespace GUI

#endif // GUI_MAKE_ITEM_WINDOW_H