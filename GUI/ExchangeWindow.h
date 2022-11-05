#ifndef GUI_EXCHANGE_WINDOW_H
#define GUI_EXCHANGE_WINDOW_H

#include <CEGUI/CEGUI.h>
#include "Artemis/Entity.h"

namespace Network
{
	class GamePacket;
}

class ItemAttributeComponent;

namespace GUI
{

class ExchangeWindow
{
public:
	ExchangeWindow();
	~ExchangeWindow();

	void HandleAskTrade(artemis::Entity& target);

	void HandleTradeRequest(Network::GamePacket& packet);
	void HandleTradeAccept(Network::GamePacket& packet);
	void HandleTradeOK(Network::GamePacket& packet);
	void HandleTradeResult(Network::GamePacket& packet);
	void HandleTradeCancel(Network::GamePacket& packet);
	void HandleTradeMoney(Network::GamePacket& packet);
	void HandleTradeItem(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);

	bool HandleClose(const CEGUI::EventArgs& e);

	bool HandleAskTradeMessageBoxResult(const CEGUI::EventArgs& e);
	bool HandleWaitTradeMessageBoxResult(const CEGUI::EventArgs& e);
	bool HandleTradeRequestMessageBoxResult(const CEGUI::EventArgs& e);

	void SendTradeAccept(unsigned char type);
	void SendTradeRequest();
	void SendTradeCancel();

	void Reset();

	bool HandleOKCheckboxStateChanged(const CEGUI::EventArgs& e);

	void SendTradeOK();

	bool HandleMoneyButtonClicked(const CEGUI::EventArgs& e);

	bool HandleMoneyInputResult(const CEGUI::EventArgs& e);

	void SendTradeMoney(unsigned int money);

	void Update();

	bool HandleDragStarted(const CEGUI::EventArgs& e);
	bool HandleDragEnded(const CEGUI::EventArgs& e);

	bool HandleScrollMy(const CEGUI::EventArgs& e);
	bool HandleScrollOther(const CEGUI::EventArgs& e);
	bool HandleScrollInventory(const CEGUI::EventArgs& e);

	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);

	bool IsSlot(const CEGUI::Window& window, const CEGUI::String& name) const;

	bool HandleMouseEnterSlotArea(const CEGUI::EventArgs& e);
	bool HandleMouseLeaveSlotArea(const CEGUI::EventArgs& e);

	bool HandleMySlotDragDropItemDropped(const CEGUI::EventArgs& e);

	bool HandleAddItemInputResult(const CEGUI::EventArgs& e);

	void SendTradeAddItem(unsigned short index, unsigned short count);

private:
	static const int EQUIPMENT_SLOT_COUNT = 10; // Taken from InventoryWindow.h
	static const int INVENTORY_SLOT_COUNT = 10;
	static const int TRADE_SLOT_COUNT = 6;

	CEGUI::Window* window_;
	CEGUI::PushButton* closeButton_;
	CEGUI::Window* myNameText_;
	CEGUI::Window* otherNameText_;
	CEGUI::Checkbox* myOKCheckbox_;
	CEGUI::Checkbox* otherOKCheckbox_;
	CEGUI::Scrollbar* myScrollbar_;
	CEGUI::Scrollbar* otherScrollbar_;
	CEGUI::Scrollbar* inventoryScrollbar_;
	CEGUI::Window* mySlots_[TRADE_SLOT_COUNT];
	CEGUI::Window* otherSlots_[TRADE_SLOT_COUNT];
	CEGUI::Window* inventorySlots_[INVENTORY_SLOT_COUNT];
	CEGUI::Window* myMoney_;
	CEGUI::Window* otherMoney_;
	CEGUI::Window* inventoryMoney_;
	CEGUI::PushButton* moneyButton_;
	CEGUI::DragContainer* dragContainer_;
	CEGUI::Window* dragItem_;
	CEGUI::Window* mySlotArea_;

	bool trading_;
	int otherServerId_;
	std::string otherName_;
	bool myOK_;
	bool otherOK_;

	bool myCanScroll_;
	bool otherCanScroll_;
	bool inventoryCanScroll_;

	bool processing_;
	ItemAttributeComponent* processItem_;
	unsigned int processItemCount_;

	typedef std::vector<ItemAttributeComponent> ItemArray;
	ItemArray myItems_;
	ItemArray otherItems_;
	ItemArray inventoryItems_;
};

} // namespace GUI

#endif // GUI_EXCHANGE_WINDOW_H