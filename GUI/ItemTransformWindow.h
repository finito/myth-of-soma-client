#ifndef GUI_ITEM_TRANSFORM_WINDOW_H
#define GUI_ITEM_TRANSFORM_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace CEGUI
{
	class CustomItemListbox;
}

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class ItemTransformWindow
{
public:
	ItemTransformWindow();
	~ItemTransformWindow();

	void HandleItemList(Network::GamePacket& packet);
	void HandleItemTransform(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);

	bool HandleCloseButtonClicked(const CEGUI::EventArgs& e);
	bool HandleApplyButtonClicked(const CEGUI::EventArgs& e);

private:
	CEGUI::Window* window_;
	CEGUI::PushButton*	closeButton_;
	CEGUI::PushButton*	applyButton_;
	CEGUI::CustomItemListbox* itemList_;
	bool				processing_;
	CEGUI::ItemEntry*	processItem_;
};

} // namespace GUI

#endif // GUI_ITEM_TRANSFORM_WINDOW_H