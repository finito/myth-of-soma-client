#ifndef GUI_COMMON_ITEM_H
#define GUI_COMMON_ITEM_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace GUI
{

// Creates the CEGUI::DragContainer to be used for dragging items around.
CEGUI::DragContainer* createItemDragContainer(const CEGUI::String& name, CEGUI::Window* dragItem);

// Creates the CEGUI::Window to be used for representation of item when dragged.
CEGUI::Window* createItemDragItem(const CEGUI::String& name);

CEGUI::String getItemImageString(const ItemAttributeComponent& item);

CEGUI::String getItemName(const ItemAttributeComponent& item);

// Sets up a CEGUI::Window to be an Item Slot.
void setItemSlot(CEGUI::Window& window, const ItemAttributeComponent* item);

// Show or hide ItemTooltip for an Item Slot.
void handleItemTooltip(CEGUI::Window* window);

bool handleItemScroll(CEGUI::Scrollbar& scrollBar, const CEGUI::EventArgs& e, const CEGUI::String& slotName);

bool handleItemDragStarted(CEGUI::DragContainer& dragContainer, CEGUI:: Window& dragItem, const CEGUI::EventArgs& e);

bool handleItemDragEnded(CEGUI::Window& dragItem, const CEGUI::EventArgs& e);

void updateItemDragContainer(CEGUI::Window& window, CEGUI::DragContainer& dragContainer, CEGUI::Window& dragItem);

bool handleMouseEnterItemSlotArea(CEGUI::DragContainer* dragContainer, CEGUI::Window* dragItem, const CEGUI::EventArgs& e);

bool handleMouseLeaveItemSlotArea(const CEGUI::EventArgs& e);

} // namespace GUI

#endif // GUI_COMMON_ITEM_H