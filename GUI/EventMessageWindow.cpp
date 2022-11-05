
#include "GUI/EventMessageWindow.h"

#include <iostream>

namespace GUI
{

using namespace CEGUI;

EventMessageWindow::EventMessageWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("eventmessagewindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&EventMessageWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&EventMessageWindow::HandleHidden, this));
		window_->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&EventMessageWindow::HandleMouseClick, this));

		text_ = window_->getChild("EventMessageWindow/Text");

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "EventMessageWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

EventMessageWindow::~EventMessageWindow()
{
	window_->destroy();
}

void EventMessageWindow::AddMessage(const std::string& message)
{
	if (messageList_.empty())
	{
		text_->setText(message);
		window_->show();
	}
	messageList_.push_back(message);
}

bool EventMessageWindow::HandleShown(const EventArgs& e)
{
	window_->moveToFront();
	window_->setModalState(true);
	return true;
}

bool EventMessageWindow::HandleHidden(const EventArgs& e)
{
	messageList_.clear();
	window_->setModalState(false);
	return true;
}

bool EventMessageWindow::HandleMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (!window_->isHit(mouseEventArgs.position))
	{
		return false;
	}

	switch (mouseEventArgs.button)
	{
	case LeftButton:
		messageList_.pop_front();
		if (!messageList_.empty())
		{
			text_->setText(messageList_.front());
		}
		else
		{
			window_->hide();
		}
		break;
	case RightButton:
		window_->hide();
		break;
	}
	return true;
}

} // namespace GUI