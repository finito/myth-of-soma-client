#ifndef GUI_EXCHANGE_TRADER_WINDOW_H
#define GUI_EXCHANGE_TRADER_WINDOW_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class ExchangeTraderWindow
{
public:
	static const int SLOT_COUNT = 10;

	ExchangeTraderWindow();
	~ExchangeTraderWindow();

	void HandleOpenExchangeTrader(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleMouseEnterSlotArea(const CEGUI::EventArgs& e);
	bool HandleMouseLeaveSlotArea(const CEGUI::EventArgs& e);
	bool HandleScrollShop(const CEGUI::EventArgs& e);
	bool HandleSlotMouseClick(const CEGUI::EventArgs& e);
	bool HandleCloseBuy(const CEGUI::EventArgs& e);
	bool HandleBuyButtonClicked(const CEGUI::EventArgs& e);
	bool HandleBuyItemInputResult(const CEGUI::EventArgs& e);

	void Update();

	void HideBuyWindow();
	void SendExchangeTraderBuy(unsigned short index, unsigned short howMany);

private:
	CEGUI::Window*		window_;
	CEGUI::PushButton*	closeButton_;
	CEGUI::Scrollbar*	scrollbar_;
	CEGUI::Window*		inventoryMoney_;
	CEGUI::Window*		slots_[SLOT_COUNT];
	CEGUI::Window*		names_[SLOT_COUNT];

	CEGUI::Window*     buyWindow_;
	CEGUI::PushButton* cancelButton_;
	CEGUI::PushButton* buyButton_;
	CEGUI::Window*	   cost_;
	CEGUI::Window*     buySlot_;
	CEGUI::Window*     required_;
	CEGUI::Window*     name_;

	typedef std::vector<ItemAttributeComponent> ItemArray;
	ItemArray items_;
};

} // namespace GUI

#endif // GUI_EXCHANGE_TRADER_WINDOW_H