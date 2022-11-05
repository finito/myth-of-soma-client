
#include "GUI/MessageBoxWindow.h"

#include <iostream>

namespace GUI
{

using namespace CEGUI;

String MessageBoxWindow::EventMessageBoxResult = "EventMessageBoxResult";

MessageBoxWindow::MessageBoxWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("messageboxwindow.layout");
		window_->hide();
		window_->setAlwaysOnTop(true);
		window_->addEvent(EventMessageBoxResult);
		window_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&MessageBoxWindow::HandleKeyPress, this));

		text_ = window_->getChild("MessageBox/Text");

		okButton_ = static_cast<PushButton*>(window_->getChild("MessageBox/OKButton"));
		okButton_->hide();
		okButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MessageBoxWindow::HandleOKButtonClicked, this));

		cancelButton_ = static_cast<PushButton*>(window_->getChild("MessageBox/CancelButton"));
		cancelButton_->hide();
		cancelButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MessageBoxWindow::HandleCancelButtonClicked, this));

		okCancelOKButton_ = static_cast<PushButton*>(window_->getChild("MessageBox/OKCancel/OKButton"));
		okCancelOKButton_->hide();
		okCancelOKButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MessageBoxWindow::HandleOKButtonClicked, this));

		okCancelCancelButton_ = static_cast<PushButton*>(window_->getChild("MessageBox/OKCancel/CancelButton"));
		okCancelCancelButton_->hide();
		okCancelCancelButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&MessageBoxWindow::HandleCancelButtonClicked, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "MessageBoxWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

MessageBoxWindow::~MessageBoxWindow()
{
	window_->destroy();
}

void MessageBoxWindow::Show(const String& message, const MessageBoxMode::Enum mode)
{
	window_->removeEvent(EventMessageBoxResult);

	text_->setText(message);
	mode_ = mode;

	switch (mode)
	{
	case MessageBoxMode::OK:
		okButton_->show();
		cancelButton_->hide();
		okCancelOKButton_->hide();
		okCancelCancelButton_->hide();
		break;
	case MessageBoxMode::Cancel:
		cancelButton_->show();
		okButton_->hide();
		okCancelOKButton_->hide();
		okCancelCancelButton_->hide();
		break;
	case MessageBoxMode::OKCancel:
		okCancelOKButton_->show();
		okCancelCancelButton_->show();
		okButton_->hide();
		cancelButton_->hide();
		break;
	default:
		break;
	}

	window_->moveToFront();
	window_->setModalState(true);
	window_->show();
}

void MessageBoxWindow::Show(const String& message, const MessageBoxMode::Enum mode, Event::Subscriber resultHandler)
{
	Show(message, mode);
	window_->subscribeEvent(EventMessageBoxResult, resultHandler);
}

void MessageBoxWindow::Hide(const bool fireEvent)
{
	window_->setModalState(false);
	window_->hide();
	if (fireEvent)
	{
		window_->fireEvent(EventMessageBoxResult, EventArgs());
	}
	window_->removeEvent(EventMessageBoxResult);
}

bool MessageBoxWindow::HandleOKButtonClicked(const EventArgs& e)
{
	result_ = MessageBoxResult::OK;
	Hide();
	return true;
}

bool MessageBoxWindow::HandleCancelButtonClicked(const EventArgs& e)
{
	result_ = MessageBoxResult::Cancel;
	Hide();
	return true;
}

MessageBoxResult::Enum MessageBoxWindow::GetResult() const
{
	return result_;
}

bool MessageBoxWindow::HandleKeyPress(const EventArgs& e)
{
	const KeyEventArgs keyArgs = static_cast<const KeyEventArgs&>(e);
	switch (mode_)
	{
	case MessageBoxMode::OK:
		if (keyArgs.scancode == Key::Escape || keyArgs.scancode == Key::Return)
		{
			okButton_->fireEvent(PushButton::EventClicked, EventArgs());
			return true;
		}
		break;
	case MessageBoxMode::Cancel:
		if (keyArgs.scancode == Key::Escape || keyArgs.scancode == Key::Return)
		{
			cancelButton_->fireEvent(PushButton::EventClicked, EventArgs());
			return true;
		}
		break;
	case  MessageBoxMode::OKCancel:
		if (keyArgs.scancode == Key::Escape)
		{
			cancelButton_->fireEvent(PushButton::EventClicked, EventArgs());
			return true;
		}
		else if (keyArgs.scancode == Key::Return)
		{
			okButton_->fireEvent(PushButton::EventClicked, EventArgs());
			return true;
		}
		break;
	}
	return false;
}

} // namespace GUI