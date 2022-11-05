
#include "GUI/MagicAbilityWindow.h"

#include "Components/InventoryComponent.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"
#include "AbilityData.h"

namespace GUI
{

using namespace CEGUI;

MagicAbilityWindow::MagicAbilityWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("magicabilitywindow.layout");
		window_->hide();

		for (int i = 0; i < MAGIC_ABILITY_SLOT_COUNT; ++i)
		{
			iconSlots_[i] = window_->getChild("MagicAbility/Icon" + PropertyHelper::intToString(i+1));
			iconSlots_[i]->setDragDropTarget(false);
			iconSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&MagicAbilityWindow::HandleMouseEnterIconArea, this));

			descriptionSlots_[i] = window_->getChild("MagicAbility/Description" + PropertyHelper::intToString(i+1));
		}

		scrollBar_ = static_cast<Scrollbar*>(window_->getChild("MagicAbility/Scrollbar"));

		closeButton_ = static_cast<PushButton*>(window_->getChild("MagicAbility/CloseButton"));

		auraButton_ = static_cast<PushButton*>(window_->getChild("MagicAbility/AuraButton"));
		blueButton_ = static_cast<PushButton*>(window_->getChild("MagicAbility/BlueButton"));
		whiteButton_ = static_cast<PushButton*>(window_->getChild("MagicAbility/WhiteButton"));
		blackButton_ = static_cast<PushButton*>(window_->getChild("MagicAbility/BlackButton"));

		window_->subscribeEvent(Window::EventShown,
			Event::Subscriber(&MagicAbilityWindow::HandleShown, this));

		window_->subscribeEvent(Window::EventMouseWheel,
			Event::Subscriber(&MagicAbilityWindow::HandleScroll, this));

		closeButton_->subscribeEvent(PushButton::EventClicked, 
			Event::Subscriber(&MagicAbilityWindow::HandleClose, this));

		auraButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicAbilityWindow::HandleAuraButton, this));

		blueButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicAbilityWindow::HandleBlueButton, this));

		whiteButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicAbilityWindow::HandleWhiteButton, this));

		blackButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicAbilityWindow::HandleBlackButton, this));

		scrollBar_->setStepSize(1.0f);
		scrollBar_->setPageSize(static_cast<float>(MAGIC_ABILITY_SLOT_COUNT));
		scrollBar_->setOverlapSize(0);
		scrollBar_->setDocumentSize(0);

		scrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&MagicAbilityWindow::HandleScrollPositionChanged, this));

		dragItem_ = WindowManager::getSingleton().createWindow("Soma/StaticImage", "MagicAbility/DragItem");
		dragItem_->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		dragItem_->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
		dragItem_->disable();

		dragContainer_ = static_cast<DragContainer*>(
			WindowManager::getSingleton().createWindow("DragContainer", "MagicAbility/DragContainer"));
		dragContainer_->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		dragContainer_->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
		dragContainer_->addChildWindow(dragItem_);
		dragContainer_->setDragAlpha(1.0f);
		dragContainer_->setProperty("MouseInputPropagationEnabled", "True");
		dragContainer_->setDragDropTarget(false);
		dragContainer_->setUserString("Type", "Ability");

		dragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&MagicAbilityWindow::HandleDragStarted, this));
		dragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&MagicAbilityWindow::HandleDragEnded, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "MagicAbilityWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

MagicAbilityWindow::~MagicAbilityWindow()
{
	dragItem_->destroy();
	dragContainer_->destroy();
	window_->destroy();
}

bool MagicAbilityWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool MagicAbilityWindow::HandleShown(const EventArgs& e)
{
	scrollBar_->setScrollPosition(0);
	Update();
	return true;
}

void MagicAbilityWindow::Update()
{
	assert(Game::Instance().myEntity);
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (!inventoryComponent)
	{
		return;
	}

	const float scrollPosition = scrollBar_->getScrollPosition();
	const float stepSize	   = scrollBar_->getStepSize();
	const float documentSize   = stepSize * std::ceilf(inventoryComponent->GetAbilitySize() / stepSize);

	scrollBar_->setDocumentSize(documentSize);
	scrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	const unsigned int startPosition = static_cast<const unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < MAGIC_ABILITY_SLOT_COUNT; ++i)
	{
		const std::size_t index = startPosition+i;
		if (index >= inventoryComponent->GetAbilitySize())
		{
			break;
		}

		const AbilityDataPtr ability = inventoryComponent->GetAbility(index);
		if (ability)
		{
			iconSlots_[i]->setUserData(ability.get());
			iconSlots_[i]->setProperty("Image", "set:abilityiconsbig image:" + PropertyHelper::intToString(ability->id_));

			descriptionSlots_[i]->setText(
				boost::str(boost::format("[font='ArimoDropShadowRegular'][colour='FFFFFF00']%1%\n"
				"[colour='FFFFFFFF']%2%") % ability->name_ % ability->description_));
		}
	}
}

bool MagicAbilityWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool MagicAbilityWindow::HandleScroll(const EventArgs& e)
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

bool MagicAbilityWindow::HandleAuraButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicAura/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicAbilityWindow::HandleBlueButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicBlue/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicAbilityWindow::HandleWhiteButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicWhite/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicAbilityWindow::HandleBlackButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicBlack/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicAbilityWindow::HandleDragStarted(const EventArgs& e)
{
	if (dragContainer_->getUserData())
	{
		const AbilityData& ability = *static_cast<AbilityData*>(dragContainer_->getUserData());
		
		if (ability.type_ == 1)
		{
			dragItem_->setProperty("Image", "set:abilityiconsbig image:" + PropertyHelper::intToString(ability.id_));
			dragItem_->setAlpha(1.0f);
			dragContainer_->setDraggingEnabled(true);
		}
		else
		{
			dragContainer_->setDraggingEnabled(false);
		}
	}
	return true;
}

bool MagicAbilityWindow::HandleDragEnded(const EventArgs& e)
{
	// The alpha is set to 0 to avoid the image of the drag item
	// being seen while not being dragged.
	dragItem_->setAlpha(0.0f);
	return true;
}

bool MagicAbilityWindow::HandleMouseEnterIconArea(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.window->getUserData())
	{
		// Must not update the drag item if a icon is currently
		// being dragged. Because do not want any changes if the list is
		// being scrolled.
		if (!dragContainer_->isBeingDragged())
		{
			const AbilityData& specialAttack = *static_cast<const AbilityData*>(mouseEventArgs.window->getUserData());
			UpdateDragContainer(*mouseEventArgs.window);
		}
	}
	return true;
}

void MagicAbilityWindow::UpdateDragContainer(Window& window)
{
	// The icon being dragged and the drag container is shared
	// therefore it needs to be placed as a child window for the
	// icon the mouse is currently within.
	dragContainer_->setUserData(window.getUserData());
	window.addChildWindow(dragContainer_);
}

} // namespace GUI