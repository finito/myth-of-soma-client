
#include "GUI/Common/Item.h"
#include "GUI/ItemTooltip.h"

#include "Components/ItemAttributeComponent.h"

#include "Game.h"

#include <boost/format.hpp>

namespace GUI
{

using namespace CEGUI;

String getItemImageString(const ItemAttributeComponent& item)
{
	std::string imageset = "itemicons";
	unsigned short pictureId = item.GetPictureId();
	if (pictureId > 5000)
	{
		pictureId -= 5000;
		imageset = "ditemicons";
	}
	const unsigned int iconGroup = ((pictureId - 1) / 81) + 1;
	const unsigned int iconIndex = ((pictureId - 1) % 81) + 1;
	return boost::str(boost::format("set:%1%%2% image:%3%") % imageset % iconGroup % iconIndex);
}

String getItemName(const ItemAttributeComponent& item)
{	
	switch (item.GetGender())
	{
	case ItemGender::Female:
		return item.GetName() + "(F)";
		break;
	case ItemGender::Male:
		return item.GetName() + "(M)";
		break;
	default:
		return item.GetName();
		break;
	}
}

void setItemSlot(Window& window, const ItemAttributeComponent* item)
{
	window.setUserData(item == nullptr ? nullptr : const_cast<ItemAttributeComponent*>(item));
	if (item == nullptr || item->Empty())
	{
		window.setProperty("Image", "");
		if (window.isChild(window.getName() + "/Quantity"))
		{
			window.getChild(window.getName() + "/Quantity")->setText("");
		}
	}
	else
	{
		window.setProperty("Image", getItemImageString(*item));
		if (window.isChild(window.getName() + "/Quantity"))
		{
			if (item->GetShopQuantity() == 0)
			{
				switch (item->GetType())
				{
				case ItemType::Potion:
				case ItemType::Food:
				case ItemType::Material:
				case ItemType::Token:
				case ItemType::Quest:
					window.getChild(window.getName() + "/Quantity")->setText(PropertyHelper::uintToString(item->GetDura()));
					break;
				default:
					window.getChild(window.getName() + "/Quantity")->setText("");
					break;
				}
			}
			else
			{
				if (item->GetShopQuantity() > 0)
				{
					window.getChild(window.getName() + "/Quantity")->setText(PropertyHelper::uintToString(item->GetShopQuantity()));
				}
				else
				{
					window.getChild(window.getName() + "/Quantity")->setText("");
				}
			}
		}
	}
}

void handleItemTooltip(Window* window)
{
	if (window == nullptr)
	{
		Game::Instance().itemTooltip->Hide();
		return;
	}

	const ItemAttributeComponent* item = static_cast<ItemAttributeComponent*>(window->getUserData());
	if (item != nullptr && !item->Empty())
	{
		UVector2 position(window->getParent()->getArea().getPosition() + window->getArea().getPosition());
		position.d_x += window->getArea().getWidth();
		Game::Instance().itemTooltip->Update(*item);
		if (window->getName().find("Repair/RepairSlot") != String::npos)
		{
			Game::Instance().itemTooltip->AppendRepairInfo(item->GetRepairCost(), item->GetRepairDuraAfter());
		}
		Game::Instance().itemTooltip->Show(position);
	}
	else
	{
		Game::Instance().itemTooltip->Hide();
	}
}

Window* createItemDragItem(const String& name)
{
	Window* dragItem = WindowManager::getSingleton().createWindow("Soma/ItemIcon", name);
	dragItem->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
	dragItem->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
	dragItem->disable();
	return dragItem;
}

DragContainer* createItemDragContainer(const String& name, Window* dragItem)
{
	DragContainer* dragContainer = static_cast<DragContainer*>(WindowManager::getSingleton().createWindow("DragContainer", name));
	dragContainer->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
	dragContainer->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
	dragContainer->addChildWindow(dragItem);
	dragContainer->setDragAlpha(1.0f);
	dragContainer->setProperty("MouseInputPropagationEnabled", "True");
	dragContainer->setDragDropTarget(false);
	return dragContainer;
}

bool handleItemScroll(Scrollbar& scrollBar, const EventArgs& e, const String& slotName)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);

	float scrollAmount = 0;
	if (mouseEventArgs.wheelChange < 0)
	{
		scrollAmount = scrollBar.getStepSize();
	}
	else if (mouseEventArgs.wheelChange > 0)
	{
		scrollAmount = -scrollBar.getStepSize();
	}

	// The inventory data must first be refreshed so that 
	// the slot will contain the correct item data.
	// The scroll position change event is taking care of the update.
	scrollBar.setScrollPosition(scrollBar.getScrollPosition() + scrollAmount);

	// Handle the case where the mouse is over an item and therefore
	// the item tooltip needs to be updated.
	String name = mouseEventArgs.window->getName();
	String::size_type found = name.find(slotName);
	if (found != String::npos)
	{
		mouseEventArgs.window->fireEvent(Window::EventMouseEntersArea, const_cast<MouseEventArgs&>(mouseEventArgs));
	}
	return true;
}

bool handleItemDragStarted(DragContainer& dragContainer, Window& dragItem, const EventArgs& e)
{	
	handleItemTooltip(nullptr);
	if (dragContainer.getUserData())
	{
		const ItemAttributeComponent& item = *static_cast<ItemAttributeComponent*>(dragContainer.getUserData());
		if (!item.Empty())
		{
			dragItem.setProperty("Image", getItemImageString(item));
			dragContainer.setDraggingEnabled(true);
			dragItem.setAlpha(1.0f);
		}
		else
		{
			dragContainer.setDraggingEnabled(false);
		}
	}
	return true;
}

bool handleItemDragEnded(Window& dragItem, const EventArgs& e)
{
	dragItem.setAlpha(0.0f);
	return true;
}

void updateItemDragContainer(Window& window, DragContainer& dragContainer, Window& dragItem)
{
	// The item being dragged and the drag container is shared for all the slots
	// therefore it needs to be placed as a child window for the slot the mouse is
	// currently within.

	// The alpha is set to 0 to avoid the image of the drag item
	// being seen while not being dragged.
	dragItem.setAlpha(0.0f);
	dragContainer.setID(window.getID());
	dragContainer.setUserData(window.getUserData());
	window.addChildWindow(&dragContainer);
}

bool handleMouseEnterItemSlotArea(DragContainer* dragContainer, Window* dragItem, const EventArgs& e)
{
	// Must not update the drag item or show item tooltip if a item is currently
	// being dragged. Because do not want any changes if the inventory is
	// being scrolled.
	if (dragContainer != nullptr && dragContainer->isBeingDragged())
	{
		return true;
	}

	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	handleItemTooltip(mouseEventArgs.window);
	if (dragContainer != nullptr && dragItem != nullptr)
	{
		updateItemDragContainer(*mouseEventArgs.window, *dragContainer, *dragItem);
	}
	return true;
}

bool handleMouseLeaveItemSlotArea(const EventArgs& e)
{
	handleItemTooltip(nullptr);
	return true;
}

} // namespace GUI