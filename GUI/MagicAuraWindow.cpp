
#include "GUI/MagicAuraWindow.h"

#include "Components/InventoryComponent.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"
#include "SpecialAttackData.h"

namespace GUI
{

using namespace CEGUI;

MagicAuraWindow::MagicAuraWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("magicaurawindow.layout");
		window_->hide();

		for (int i = 0; i < MAGIC_AURA_SLOT_COUNT; ++i)
		{
			iconSlots_[i] = window_->getChild("MagicAura/Icon" + PropertyHelper::intToString(i+1));
			iconSlots_[i]->setDragDropTarget(false);
			iconSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&MagicAuraWindow::HandleMouseEnterIconArea, this));

			descriptionSlots_[i] = window_->getChild("MagicAura/Description" + PropertyHelper::intToString(i+1));
		}

		scrollBar_ = static_cast<Scrollbar*>(window_->getChild("MagicAura/Scrollbar"));

		closeButton_ = static_cast<PushButton*>(window_->getChild("MagicAura/CloseButton"));

		blueButton_ = static_cast<PushButton*>(window_->getChild("MagicAura/BlueButton"));
		whiteButton_ = static_cast<PushButton*>(window_->getChild("MagicAura/WhiteButton"));
		blackButton_ = static_cast<PushButton*>(window_->getChild("MagicAura/BlackButton"));
		abilityButton_ = static_cast<PushButton*>(window_->getChild("MagicAura/AbilityButton"));

		window_->subscribeEvent(Window::EventShown,
			Event::Subscriber(&MagicAuraWindow::HandleShown, this));

		window_->subscribeEvent(Window::EventMouseWheel,
			Event::Subscriber(&MagicAuraWindow::HandleScroll, this));

		closeButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicAuraWindow::HandleClose, this));

		blueButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicAuraWindow::HandleBlueButton, this));

		whiteButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MagicAuraWindow::HandleWhiteButton, this));

		blackButton_->subscribeEvent(PushButton::EventClicked,
			 Event::Subscriber(&MagicAuraWindow::HandleBlackButton, this));

		abilityButton_->subscribeEvent(PushButton::EventClicked,
			 Event::Subscriber(&MagicAuraWindow::HandleAbilityButton, this));

		scrollBar_->setStepSize(1.0f);
		scrollBar_->setPageSize(static_cast<float>(MAGIC_AURA_SLOT_COUNT));
		scrollBar_->setOverlapSize(0);
		scrollBar_->setDocumentSize(0);

		scrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&MagicAuraWindow::HandleScrollPositionChanged, this));

		dragItem_ = WindowManager::getSingleton().createWindow("Soma/StaticImage", "MagicAura/DragItem");
		dragItem_->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		dragItem_->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
		dragItem_->disable();

		dragContainer_ = static_cast<DragContainer*>(
			WindowManager::getSingleton().createWindow("DragContainer", "MagicAura/DragContainer"));
		dragContainer_->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		dragContainer_->setSize(UVector2(UDim(1, 0), UDim(1, 0)));
		dragContainer_->addChildWindow(dragItem_);
		dragContainer_->setDragAlpha(1.0f);
		dragContainer_->setProperty("MouseInputPropagationEnabled", "True");
		dragContainer_->setDragDropTarget(false);
		dragContainer_->setUserString("Type", "SpecialAttack");

		dragContainer_->subscribeEvent(DragContainer::EventDragStarted,
			Event::Subscriber(&MagicAuraWindow::HandleDragStarted, this));
		dragContainer_->subscribeEvent(DragContainer::EventDragEnded,
			Event::Subscriber(&MagicAuraWindow::HandleDragEnded, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "MagicAuraWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

MagicAuraWindow::~MagicAuraWindow()
{
	dragItem_->destroy();
	dragContainer_->destroy();
	window_->destroy();
}

bool MagicAuraWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool MagicAuraWindow::HandleShown(const EventArgs& e)
{
	scrollBar_->setScrollPosition(0);
	Update();
	return true;
}

void MagicAuraWindow::Update()
{
	assert(Game::Instance().myEntity);
	InventoryComponent* inventoryComponent = Game::Instance().myEntity->getComponent<InventoryComponent>();
	if (!inventoryComponent)
	{
		return;
	}

	const float scrollPosition = scrollBar_->getScrollPosition();
	const float stepSize	   = scrollBar_->getStepSize();
	const float documentSize   = stepSize * std::ceilf(inventoryComponent->GetSpecialAttackSize() / stepSize);

	scrollBar_->setDocumentSize(documentSize);
	scrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	const unsigned int startPosition = static_cast<const unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < MAGIC_AURA_SLOT_COUNT; ++i)
	{
		const std::size_t index = startPosition+i;
		if (index >= inventoryComponent->GetSpecialAttackSize())
		{
			break;
		}

		const SpecialAttackDataPtr specialAttack = inventoryComponent->GetSpecialAttackByIndex(index);
		iconSlots_[i]->setUserData(specialAttack.get());
		iconSlots_[i]->setProperty("Image", "set:specialattackiconsbig image:" + PropertyHelper::intToString(specialAttack->id_));

		std::string healthCost;
		if (specialAttack->healthCost_ >= 0)
		{
			healthCost = boost::str(boost::format("HP %1%") % specialAttack->healthCost_);
		}

		std::string manaCost;
		if (specialAttack->manaCost_ >= 0)
		{
			manaCost = boost::str(boost::format("MP %1%") % specialAttack->manaCost_);
		}

		std::string description;
		if (specialAttack->duration_ == 0)
		{
			if (specialAttack->range_ == 0)
			{
				description = boost::str(boost::format(
					"[font='ArimoDropShadowRegular'][colour='FFFFFF00']%1%(%2%)\n"
					"[colour='FFFFFFFF']%3%\nSkill %4% Power %5% %6% %7%")
					% specialAttack->name_ % specialAttack->WeaponTypeToString() % specialAttack->description_
					% specialAttack->requirementSkill_ % specialAttack->damage_ % healthCost % manaCost);
			}
			else
			{
				description = boost::str(boost::format(
					"[font='ArimoDropShadowRegular'][colour='FFFFFF00']%1%(%2%)\n"
					"[colour='FFFFFFFF']%3%\nRange %4% Skill %5%\nPower %6% %7% %8%")
					% specialAttack->name_ % specialAttack->WeaponTypeToString() % specialAttack->description_
					% specialAttack->range_ % specialAttack->requirementSkill_ % specialAttack->damage_
					% healthCost % manaCost);
			}
		}
		else
		{
			if (specialAttack->range_ == 0)
			{
				description = boost::str(boost::format(
					"[font='ArimoDropShadowRegular'][colour='FFFFFF00']%1%(%2%)\n"
					"[colour='FFFFFFFF']%3%\nDuration %4% Skill %5%\nPower %6% %7% %8%")
					% specialAttack->name_ % specialAttack->WeaponTypeToString() % specialAttack->description_
					% (specialAttack->duration_/1000) % specialAttack->requirementSkill_ % specialAttack->damage_
					% healthCost % manaCost);
			}
			else
			{
				description = boost::str(boost::format(
					"[font='ArimoDropShadowRegular'][colour='FFFFFF00']%1%(%2%)\n"
					"[colour='FFFFFFFF']%3%\nRange %4% Duration %5%\nSkill %6% Power %7% %8% %9%")
					% specialAttack->name_ % specialAttack->WeaponTypeToString() % specialAttack->description_
					% specialAttack->range_ % (specialAttack->duration_/1000) % specialAttack->requirementSkill_
					% specialAttack->damage_ % healthCost % manaCost);
			}
		}
		descriptionSlots_[i]->setText(description);
	}
}

bool MagicAuraWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool MagicAuraWindow::HandleScroll(const EventArgs& e)
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

bool MagicAuraWindow::HandleBlueButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicBlue/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicAuraWindow::HandleWhiteButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicWhite/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicAuraWindow::HandleBlackButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicBlack/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicAuraWindow::HandleAbilityButton(const EventArgs& e)
{
	window_->hide();
	Window* frame = System::getSingleton().getGUISheet()->getChild("MagicAbility/Frame");
	frame->show();
	frame->moveInFront(System::getSingleton().getGUISheet()->getChild("FocusRoot"));
	frame->activate();
	return true;
}

bool MagicAuraWindow::HandleDragStarted(const EventArgs& e)
{
	if (dragContainer_->getUserData())
	{
		const SpecialAttackData& specialAttack = *static_cast<SpecialAttackData*>(dragContainer_->getUserData());

		dragItem_->setProperty("Image", "set:specialattackiconsbig image:" + PropertyHelper::intToString(specialAttack.id_));
		dragItem_->setAlpha(1.0f);
	}

	return true;
}

bool MagicAuraWindow::HandleDragEnded(const EventArgs& e)
{
	// The alpha is set to 0 to avoid the image of the drag item
	// being seen while not being dragged.
	dragItem_->setAlpha(0.0f);
	return true;
}

bool MagicAuraWindow::HandleMouseEnterIconArea(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.window->getUserData())
	{
		// Must not update the drag item if a icon is currently
		// being dragged. Because do not want any changes if the list is
		// being scrolled.
		if (!dragContainer_->isBeingDragged())
		{
			const SpecialAttackData& specialAttack = *static_cast<const SpecialAttackData*>(mouseEventArgs.window->getUserData());
			UpdateDragContainer(*mouseEventArgs.window);
		}
	}
	return true;
}

void MagicAuraWindow::UpdateDragContainer(Window& window)
{
	// The icon being dragged and the drag container is shared
	// therefore it needs to be placed as a child window for the
	// icon the mouse is currently within.
	dragContainer_->setUserData(window.getUserData());
	window.addChildWindow(dragContainer_);
}

} // namespace GUI