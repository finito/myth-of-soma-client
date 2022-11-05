#ifndef GUI_GUILD_STASH_WINDOW_H
#define GUI_GUILD_STASH_WINDOW_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class GuildStashWindow
{
public:
	static const int EQUIPMENT_SLOT_COUNT = 10; // Taken from InventoryWindow.h
	static const int INVENTORY_SLOT_COUNT = 10;
	static const int STORAGE_SLOT_COUNT = 10;

	static CEGUI::String EventInventoryChanged;

	GuildStashWindow();
	~GuildStashWindow();

	void HandleOpenGuildStorage(Network::GamePacket& packet);

	void HandlePutItem(Network::GamePacket& packet);
	void HandleGetItem(Network::GamePacket& packet);

	void HandleLimitChange(Network::GamePacket& packet);

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
	bool HandleInventoryChanged(const CEGUI::EventArgs& e);
	bool HandlePutItemInputResult(const CEGUI::EventArgs& e);
	bool HandleGetItemInputResult(const CEGUI::EventArgs& e);
	bool HandleLimitCheckboxChanged(const CEGUI::EventArgs& e);

	void Update();

	void SendPutItem(unsigned short index, unsigned short howMany);
	void SendGetItem(unsigned short index, unsigned short howMany);
	void SendCloseStorage();

private:
	CEGUI::Window*		window_;
	CEGUI::PushButton*	closeButton_;
	CEGUI::PushButton*	putItemButton_;
	CEGUI::PushButton*	getItemButton_;
	CEGUI::Checkbox*	limitCheckbox_;
	CEGUI::PushButton*	logButton_;
	CEGUI::Scrollbar*	inventoryScrollBar_;
	CEGUI::Scrollbar*	storageScrollBar_;
	CEGUI::Window*		inventorySlots_[INVENTORY_SLOT_COUNT];
	CEGUI::Window*		storageSlots_[STORAGE_SLOT_COUNT];
	CEGUI::Window*		selectedSlot_;
	CEGUI::Window*		slotSelection_;
	CEGUI::Window*      guildName_;
	CEGUI::Window*      guildEmblem_;
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

#endif // GUI_GUILD_STASH_WINDOW_H