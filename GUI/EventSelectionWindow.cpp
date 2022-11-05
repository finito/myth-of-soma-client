
#include "GUI/EventSelectionWindow.h"

#include "Network/GameProtocol.h"
#include "Network/GamePacket.h"
#include "Network/GameSocket.h"

#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

EventSelectionWindow::EventSelectionWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("eventselectionwindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&EventSelectionWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&EventSelectionWindow::HandleHidden, this));
		window_->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&EventSelectionWindow::HandleMouseClick, this));

		text_ = window_->getChild("EventSelectionWindow/Text");
		text_->hide();

		for (int i = 0; i < SELECTION_COUNT; ++i)
		{
			selections_[i] = window_->getChild("EventSelectionWindow/Selection" + PropertyHelper::intToString(i+1));
			selections_[i]->setID(i);
			selections_[i]->hide();
			selections_[i]->setText("");
			selections_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&EventSelectionWindow::HandleSelectionMouseClick, this));
			selections_[i]->subscribeEvent(Window::EventMouseEnters,
				Event::Subscriber(&EventSelectionWindow::HandleSelectionMouseEnters, this));
			selections_[i]->subscribeEvent(Window::EventMouseLeaves,
				Event::Subscriber(&EventSelectionWindow::HandleSelectionMouseLeaves, this));
		}

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "EventSelectionWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

EventSelectionWindow::~EventSelectionWindow()
{
	window_->destroy();
}

void EventSelectionWindow::SetMessage(const std::string& message)
{
	text_->hide();
	text_->setText(message);
}

void EventSelectionWindow::SetSelection(const std::string& selection, const int index)
{
	if (index < 0 || index >= SELECTION_COUNT)
	{
		return;
	}

	selections_[index]->hide();
	selections_[index]->setText(selection);
	const float width = PropertyHelper::stringToFloat(selections_[index]->getProperty("HorzExtent"));
	const float height = PropertyHelper::stringToFloat(selections_[index]->getProperty("VertExtent"));
	selections_[index]->setHeight(UDim(0, height));
	selections_[index]->setWidth(UDim(0, width));
}

void EventSelectionWindow::ShowSelections()
{
	for (int i = 0; i < SELECTION_COUNT; ++i)
	{
		if (!selections_[i]->getText().empty())
		{
			selections_[i]->show();
		}
	}
}

bool EventSelectionWindow::HandleShown(const EventArgs& e)
{
	window_->moveToFront();
	window_->setModalState(true);
	if (text_->getText().empty())
	{
		ShowSelections();
	}
	else
	{
		text_->show();
	}
	return true;
}

bool EventSelectionWindow::HandleHidden(const EventArgs& e)
{
	for (int i = 0; i < SELECTION_COUNT; ++i)
	{
		selections_[i]->hide();
		selections_[i]->setText("");
	}
	text_->hide();
	text_->setText("");
	window_->setModalState(false);
	return true;
}

bool EventSelectionWindow::HandleMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (!window_->isHit(mouseEventArgs.position))
	{
		return false;
	}

	switch (mouseEventArgs.button)
	{
	case LeftButton:
		if (text_->isVisible())
		{
			text_->hide();
			ShowSelections();
		}
		break;
	case RightButton:
		window_->hide();
		break;
	}
	return true;
}

bool EventSelectionWindow::HandleSelectionMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		SendSelection(mouseEventArgs.window->getID());
		window_->hide();
	}
	return true;
}

void EventSelectionWindow::SendSelection(const unsigned int selectionNumber)
{
	GamePacket packet(GameProtocol::PKT_EVENTSELBOX);
	packet << static_cast<sf::Uint16>(selectionNumber);
	Game::Instance().gameSocket->Send(packet);	
}

bool EventSelectionWindow::HandleSelectionMouseEnters(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	mouseEventArgs.window->setProperty("BackgroundEnabled", "True");
	return true;
}

bool EventSelectionWindow::HandleSelectionMouseLeaves(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	mouseEventArgs.window->setProperty("BackgroundEnabled", "False");
	return true;
}

} // namespace GUI