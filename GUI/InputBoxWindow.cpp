
#include "GUI/InputBoxWindow.h"

#include <iostream>

namespace GUI
{

using namespace CEGUI;

String InputBoxWindow::EventInputBoxResult = "EventInputBoxResult";

InputBoxWindow::InputBoxWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("inputboxwindow.layout");
		window_->hide();
		window_->setAlwaysOnTop(true);
		window_->addEvent(EventInputBoxResult);
		window_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&InputBoxWindow::HandleKeyPress, this));

		titleText_ = window_->getChild("InputBox/Title");

		inputEditbox_ = static_cast<Editbox*>(window_->getChild("InputBox/Input"));
		inputEditbox_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&InputBoxWindow::HandleKeyPress, this));

		okButton_ = static_cast<PushButton*>(window_->getChild("InputBox/OKButton"));
		okButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&InputBoxWindow::HandleOKButtonClicked, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "InputBoxWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

InputBoxWindow::~InputBoxWindow()
{
	window_->destroy();
}

void InputBoxWindow::Show(const std::string& title, Event::Subscriber resultHandler)
{
	userData_ = nullptr;

	titleText_->setText(title);

	inputEditbox_->setText("");

	window_->removeEvent(EventInputBoxResult);
	window_->subscribeEvent(EventInputBoxResult, resultHandler);
	window_->moveToFront();
	window_->setModalState(true);
	window_->show();

	inputEditbox_->activate();
}

void InputBoxWindow::Hide(const bool fireResult)
{
	window_->setModalState(false);
	window_->hide();
	if (fireResult)
	{
		window_->fireEvent(EventInputBoxResult, EventArgs());
	}
	window_->removeEvent(EventInputBoxResult);
}

bool InputBoxWindow::HandleOKButtonClicked(const EventArgs& e)
{
	Hide();
	return true;
}

bool InputBoxWindow::HandleKeyPress(const EventArgs& e)
{
	const KeyEventArgs keyArgs = static_cast<const KeyEventArgs&>(e);
	switch (keyArgs.scancode)
	{
	case Key::Escape:
		Hide(false);
		break;
	case Key::Return:
		okButton_->fireEvent(PushButton::EventClicked, EventArgs());
		return true;
	}
	return false;
}

const String InputBoxWindow::GetInputText() const
{
	return inputEditbox_->getText();
}

void InputBoxWindow::SetInputText(const String value)
{
	inputEditbox_->setText(value);
	inputEditbox_->setSelection(0, value.length());
}

void InputBoxWindow::SetUserData(void* value)
{
	userData_ = value;
}

void* InputBoxWindow::GetUserData() const
{
	return userData_;
}

} // namespace GUI