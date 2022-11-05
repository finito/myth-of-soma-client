
#include "GUI/MagicBlueWindow.h"

#include "Components/InventoryComponent.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"
#include "MagicData.h"

namespace GUI
{

using namespace CEGUI;

MagicBlueWindow::MagicBlueWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("magicbluewindow.layout");
		window_->hide();

		for (int i = 0; i < MAGIC_BLUE_SLOT_COUNT; ++i)
		{
			iconSlots_[i] = window_->getChild("MagicBlue/Icon" + PropertyHelper::intToString(i+1));
			iconSlots_[i]->setDragDropTarget(false);
			iconSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&MagicBlueWindow::HandleMouseEnterIconArea, this));

			descriptionSlots_[i] = window_->getChild("MagicBlue/Description" + PropertyHelper::intToString(i+1));
		}

		scrollBar_ = static_cast<Scrollbar*>(window_->getChild("MagicBlue/Scrollbar"));

		closeButton_ = static_cast<PushButton*>(window_->getChild("MagicBlue/CloseButton"));

		auraButton_ = static_cast<PushButton*>(window_->getChild("MagicBlue/AuraButton"));
		whiteButton_ = static_cast<PushButton*>(window_->getChild("MagicBlue/WhiteButton"));
		blackButton_ = static_cast<PushButton*>(window_->getChild("MagicBlue/BlackButton"));
		abilityButton_ = static_cast<PushButton*>(window_->getChild("MagicBlue/AbilityButton"));

		window_->subscribeEvent(Window::EventShown,
			Event::Subscriber(&MagicBlueWindow::HandleShown, this));

		window_->subscribeEvent(Window::EventMouseWheel,
			Event::Subscriber(&MagicBlueWindow::HandleScroll, this));

		closeButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicBlueWindow::HandleClose, this));

		auraButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicBlueWindow::HandleAuraButton, this));

		whiteButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicBlueWindow::HandleWhiteButton, this));

		blackButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicBlueWindow::HandleBlackButton, this));

		abilityButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicBlueWindow::HandleAbilityButton, this));

		scrollBar_->setStepSize(1.0f);
		scrollBar_->setPageSize(static_cast<float>(MAGIC_BLUE_SLOT_COUNT));
		scrollBar_->setOverlapSize(0);
		scrollBar_->setDocumentSize(0);

		scrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&MagicBlueWindow::HandleScrollPositionChanged, this));

		dragItem_ = WindowManager::getSingleton().createWindow("Soma/StaticImage", "MagicBlue/DragItem");
		dragItem_->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		dragItem_->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
		dragItem_->disable();

		dragContainer_ = static_cast<DragContainer*>(
			WindowManager::getSingleton().createWindow("DragContainer", "MagicBlue/DragContainer"));
		dragContainer_->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		dragContainer_->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
		dragContainer_->addChildWindow(dragItem_);
		dragContainer_->setDragAlpha(1.0f);
		dragContainer_->setProperty("MouseInputPropagationEnabled", "True");
		dragContainer_->setDragDropTarget(false);
		dragContainer_->setUserString("Type", "Magic");

		dragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&MagicBlueWindow::HandleDragStarted, this));
		dragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&MagicBlueWindow::HandleDragEnded, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "MagicBlueWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

MagicBlueWindow::~MagicBlueWindow()
{
	dragItem_->destroy();
	dragContainer_->destroy();
	window_->destroy();
}

bool MagicBlueWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool MagicBlueWindow::HandleShown(const EventArgs& e)
{
	scrollBar_->setScrollPosition(0);
	Update();
	return true;
}

void MagicBlueWindow::Update()
{
	assert(Game::Instance().myEntity);
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (!inventoryComponent)
	{
		return;
	}

	const float scrollPosition = scrollBar_->getScrollPosition();
	const float stepSize	   = scrollBar_->getStepSize();
	const float documentSize   = stepSize * std::ceilf(inventoryComponent->GetBlueMagicSize() / stepSize);

	scrollBar_->setDocumentSize(documentSize);
	scrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	const unsigned int startPosition = static_cast<const unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < MAGIC_BLUE_SLOT_COUNT; ++i)
	{
		const std::size_t index = startPosition+i;
		if (index >= inventoryComponent->GetBlueMagicSize())
		{
			break;
		}

		const MagicDataPtr magic = inventoryComponent->GetBlueMagic(index);
		iconSlots_[i]->setUserData(magic.get());
		iconSlots_[i]->setProperty("Image", "set:magiciconsbig image:" + PropertyHelper::intToString(magic->id_));

		std::string description;
		if (magic->duration_ == 0)
		{
			description = boost::str(boost::format(
				"[font='ArimoDropShadowRegular'][colour='FFFFFF00']%1%\n"
				"[colour='FFFFFFFF']%2%\nRange %3% Power %4% MP %5%")
				% magic->name_ % magic->description_ % magic->range_ % magic->damage_ % magic->manaCost_);
		}
		else
		{
			description = boost::str(boost::format(
				"[font='ArimoDropShadowRegular'][colour='FFFFFF00']%1%\n"
				"[colour='FFFFFFFF']%2%\nRange %3% Duration %4% Power %5% MP %6%")
				% magic->name_ % magic->description_ % magic->range_ % magic->duration_ % magic->damage_
				% magic->manaCost_);
		}
		descriptionSlots_[i]->setText(description);
	}
}

bool MagicBlueWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool MagicBlueWindow::HandleScroll(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);

	float scrollAmount = 0;
	if (mouseEventArgs.wheelChange < 0)
	{
		scrollAmount = scrollBar_->getStepSize();
	}
	else if (mouseEventArgs.wheelChange > 0)
	{
		scrollAmount = -scrollBar_->getStepSize();
	}

	scrollBar_->setScrollPosition(scrollBar_->getScrollPosition() + scrollAmount);
	return true;
}

bool MagicBlueWindow::HandleAuraButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicAura/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicBlueWindow::HandleWhiteButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicWhite/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicBlueWindow::HandleBlackButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicBlack/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicBlueWindow::HandleAbilityButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicAbility/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicBlueWindow::HandleDragStarted(const EventArgs& e)
{
	if (dragContainer_->getUserData())
	{
		const MagicData& magic = *static_cast<MagicData*>(dragContainer_->getUserData());
		
		dragItem_->setProperty("Image", "set:magiciconsbig image:" + PropertyHelper::intToString(magic.id_));
		dragItem_->setAlpha(1.0f);
	}
	return true;
}

bool MagicBlueWindow::HandleDragEnded(const EventArgs& e)
{
	// The alpha is set to 0 to avoid the image of the drag item
	// being seen while not being dragged.
	dragItem_->setAlpha(0.0f);
	return true;
}

bool MagicBlueWindow::HandleMouseEnterIconArea(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.window->getUserData())
	{
		// Must not update the drag item if a icon is currently
		// being dragged. Because do not want any changes if the list is
		// being scrolled.
		if (!dragContainer_->isBeingDragged())
		{
			const MagicData& magic = *static_cast<const MagicData*>(mouseEventArgs.window->getUserData());
			UpdateDragContainer(*mouseEventArgs.window);
		}
	}
	return true;
}

void MagicBlueWindow::UpdateDragContainer(Window& window)
{
	// The icon being dragged and the drag container is shared
	// therefore it needs to be placed as a child window for the
	// icon the mouse is currently within.
	dragContainer_->setUserData(window.getUserData());
	window.addChildWindow(dragContainer_);
}

} // namespace GUI