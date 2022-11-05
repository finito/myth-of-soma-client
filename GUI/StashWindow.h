#ifndef GUI_STASH_WINDOW_H
#define GUI_STASH_WINDOW_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class StashWindow
{
public:
	static const int EQUIPMENT_SLOT_COUNT = 10; // Taken from InventoryWindow.h
	static const int INVENTORY_SLOT_COUNT = 10;
	static const int STORAGE_SLOT_COUNT = 10;

	static CEGUI::String EventInventoryChanged;

	StashWindow();
	~StashWindow();

	void HandleOpenStorage(Network::GamePacket& packet);

	void HandlePutItem(Network::GamePacket& packet);
	void HandleGetItem(Network::GamePacket& packet);
	void HandlePutGetMoney(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleMouseEnterSlotArea(const CEGUI::EventArgs& e);
	bool HandleMouseLeaveSlotArea(const CEGUI::EventArgs& e);
	bool HandleSlotMouseClick(const CEGUI::EventArgs& e);
	bool HandleScrollInventory(const CEGUI::EventArgs& e);
	bool HandleScrollStorage(const CEGUI::EventArgs& e);
	bool HandlePutItemButtonClicked(const CEGUI::EventArgs& e);
	bool HandleGetItemButtonClicked(const CEGUI::EventArgs& e);
	bool HandlePutMoneyButtonClicked(const CEGUI::EventArgs& e);
	bool HandleGetMoneyButtonClicked(const CEGUI::EventArgs& e);
	bool HandleInventoryChanged(const CEGUI::EventArgs& e);
	bool HandlePutMoneyInputResult(const CEGUI::EventArgs& e);
	bool HandleGetMoneyInputResult(const CEGUI::EventArgs& e);
	bool HandlePutItemInputResult(const CEGUI::EventArgs& e);
	bool HandleGetItemInputResult(const CEGUI::EventArgs& e);

	void Update();

	void SendPutItem(unsigned short index, unsigned short howMany);
	void SendGetItem(unsigned short index, unsigned short howMany);

	void SendPutMoney(unsigned int money);
	void SendGetMoney(unsigned int money);

private:
	CEGUI::Window*		window_;
	CEGUI::PushButton*	closeButton_;
	CEGUI::PushButton*	putItemButton_;
	CEGUI::PushButton*	getItemButton_;
	CEGUI::PushButton*	putMoneyButton_;
	CEGUI::PushButton*	getMoneyButton_;
	CEGUI::Scrollbar*	inventoryScrollBar_;
	CEGUI::Scrollbar*	storageScrollBar_;
	CEGUI::Window*		inventoryMoney_;
	CEGUI::Window*		storageMoney_;
	CEGUI::Window*		inventorySlots_[INVENTORY_SLOT_COUNT];
	CEGUI::Window*		storageSlots_[STORAGE_SLOT_COUNT];
	CEGUI::Window*		selectedSlot_;
	CEGUI::Window*		slotSelection_;
	const ItemAttributeComponent* selectedItem_;
	bool				inventoryCanScroll_;
	bool				storageCanScroll_;
	bool				processing_;
	unsigned int		itemCountMax_;

	typedef std::vector<ItemAttributeComponent> ItemArray;
	ItemArray storageItems_;

	typedef std::vector<ItemAttributeComponent*> ItemPtrArray;
	ItemPtrArray inventoryItems_;
};

} // namespace GUI

#endif // GUI_STASH_WINDOW_H