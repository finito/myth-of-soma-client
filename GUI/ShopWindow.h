#ifndef GUI_SHOP_WINDOW_H
#define GUI_SHOP_WINDOW_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace Network
{
	class GamePacket;
}

namespace GUI
{

namespace ShopType
{
	enum Enum
	{
		Weapon = 6,
		Armor,
		Accessory,
		Potion,
		Food,
		Lottery,
		General,
		First = Weapon,
		Last  = General + 1,
	};
}

class ShopWindow
{
public:
	static const int EQUIPMENT_SLOT_COUNT = 10; // Taken from InventoryWindow.h
	static const int INVENTORY_SLOT_COUNT = 10;
	static const int SHOP_SLOT_COUNT = 10;

	static CEGUI::String EventShopBuy;
	static CEGUI::String EventShopSell;
	static CEGUI::String EventInventoryChanged;

	ShopWindow();
	~ShopWindow();

	void HandleOpenShop(Network::GamePacket& packet);
	void HandleShopItemQuantity(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleMouseEnterSlotArea(const CEGUI::EventArgs& e);
	bool HandleMouseLeaveSlotArea(const CEGUI::EventArgs& e);
	bool HandleSlotMouseClick(const CEGUI::EventArgs& e);
	bool HandleScrollInventory(const CEGUI::EventArgs& e);
	bool HandleScrollShop(const CEGUI::EventArgs& e);
	bool HandleSellButtonClicked(const CEGUI::EventArgs& e);
	bool HandleBuyButtonClicked(const CEGUI::EventArgs& e);
	bool HandleShopSell(const CEGUI::EventArgs& e);
	bool HandleShopBuy(const CEGUI::EventArgs& e);
	bool HandleInventoryChanged(const CEGUI::EventArgs& e);
	bool HandleSellItemInputResult(const CEGUI::EventArgs& e);
	bool HandleBuyItemInputResult(const CEGUI::EventArgs& e);

	void Update();

	void SetType(unsigned int shopType);

	void SendShopSell(unsigned short index, unsigned short howMany);
	void SendShopBuy(unsigned short index, unsigned short howMany);

private:
	CEGUI::Window*		window_;
	CEGUI::PushButton*	closeButton_;
	CEGUI::PushButton*	sellButton_;
	CEGUI::PushButton*	buyButton_;
	CEGUI::Scrollbar*	inventoryScrollBar_;
	CEGUI::Scrollbar*	shopScrollBar_;
	CEGUI::Window*		money_;
	CEGUI::Window*		cost_;
	CEGUI::Window*		inventorySlots_[INVENTORY_SLOT_COUNT];
	CEGUI::Window*		shopSlots_[SHOP_SLOT_COUNT];
	CEGUI::Window*		selectedSlot_;
	CEGUI::Window*		slotSelection_;
	CEGUI::Window*		shopTypeImage_;
	ShopType::Enum		shopType_;
	unsigned int		shopNumber_;
	unsigned int		buyRate_;
	unsigned int		sellRate_;
	const ItemAttributeComponent* selectedItem_;
	bool				processing_; // Set to true when buying or selling an item. Set false when updated after inventory is changed.
	bool				inventoryCanScroll_;
	bool				shopCanScroll_;

	typedef std::vector<ItemAttributeComponent> ItemArray;
	ItemArray shopItems_;

	typedef std::vector<ItemAttributeComponent*> ItemPtrArray;
	ItemPtrArray inventoryItems_;
};

} // namespace GUI

#endif // GUI_SHOP_WINDOW_H