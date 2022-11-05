#ifndef GUI_ABILITY_WINDOW_H
#define GUI_ABILITY_WINDOW_H

#include <CEGUI/CEGUI.h>
#include "Components/ItemAttributeComponent.h"

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class AbilityWindow
{
public:
	static CEGUI::String EventInventoryChanged;

	AbilityWindow();
	~AbilityWindow();

	void HandleAbilityOpen(Network::GamePacket& packet);
	void HandleProduceItem(Network::GamePacket& packet);

private:
	bool HandleOpen(const CEGUI::EventArgs& e);
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

	void Update();
	void UpdateItemData();

	void MoveItemToMaterialSlot(ItemAttributeComponent& item, const unsigned short count);
	void MoveItemToInventorySlot(const ItemAttributeComponent& item);

	void SendProduceSynthesis();
	void SendProduceOther();
	void SendProduce();

private:
	static const int EQUIPMENT_SLOT_COUNT = 10; // Taken from InventoryWindow.h
	static const int INVENTORY_SLOT_COUNT = 10;

	static const int MATERIAL_ITEM_COUNT = 5;

	CEGUI::Window*		  window_;
	CEGUI::Window*        resultDescription_;
	CEGUI::Window*        resultSlot_;
	CEGUI::PushButton*	  okButton_;
	CEGUI::PushButton*    produceButton_;
	CEGUI::Scrollbar*	  inventoryScrollBar_;
	CEGUI::Window*		  inventorySlots_[INVENTORY_SLOT_COUNT];
	CEGUI::RadioButton*   separationButton_;
	CEGUI::RadioButton*   compositionButton_;
	CEGUI::RadioButton*   partitionButton_;
	CEGUI::RadioButton*   upgradeButton_;
	CEGUI::Window*        materialSlots_[MATERIAL_ITEM_COUNT];
	CEGUI::Window*        materialSlotArea_;
	bool				  processing_;
	CEGUI::DragContainer* dragContainer_;
	CEGUI::Window*		  dragItem_;

	typedef std::vector<ItemAttributeComponent> ItemArray;
	ItemArray inventoryItems_;
	ItemArray materialItems_;
	ItemAttributeComponent resultItem_;

	unsigned char abilityType_;
	CEGUI::String abilityTypeString_;
};

} // namespace GUI

#endif // GUI_ABILITY_WINDOW_H