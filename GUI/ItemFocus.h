#ifndef GUI_ITEM_FOCUS_H
#define GUI_ITEM_FOCUS_H

#include <CEGUI/CEGUI.h>
#include "Artemis/Entity.h"

namespace GUI
{

class ItemFocus
{
public:
	ItemFocus();
	~ItemFocus();

	void Update(artemis::Entity& item);
	void Show(const CEGUI::UVector2& textPosition, const CEGUI::UVector2& itemPosition);
	void Hide();

	void SetCanPickup(bool canPickup);

private:
	bool HandleMouseLeaveArea(const CEGUI::EventArgs& e);
	bool HandleMouseClick(const CEGUI::EventArgs& e);

private:
	CEGUI::Window* window_;
	CEGUI::Window* frame_;
	CEGUI::Window* nameText_;
	CEGUI::Window* itemArea_;

	artemis::Entity* itemEntity_;
	bool canPickup_;
};

} // namespace GUI

#endif // GUI_ITEM_FOCUS_H