
#include "GUI/LoginWindow.h"
#include <iostream>

namespace GUI
{

using namespace CEGUI;

LoginWindow::LoginWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("loginwindow.layout");
		window_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&LoginWindow::HandleKeyPress, this));

		AnimationManager::getSingletonPtr()->loadAnimationsFromXML("loginwindow.animation");

		usernameEditbox_ = static_cast<Editbox*>(window_->getChild("Login/UsernameEditbox"));
		usernameEditbox_->activate();
		usernameEditbox_->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&LoginWindow::HandleKeyPress, this));

		passwordEditbox_ = static_cast<Editbox*>(window_->getChild("Login/PasswordEditbox"));
		passwordEditbox_->setTextMasked(true);
		passwordEditbox_->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&LoginWindow::HandleKeyPress, this));

		okButton_	    = static_cast<PushButton*>(window_->getChild("Login/OKButton"));
		cancelButton_	= static_cast<PushButton*>(window_->getChild("Login/CancelButton"));
		accountButton_  = static_cast<PushButton*>(window_->getChild("Login/AccountButton"));
		registerButton_ = static_cast<PushButton*>(window_->getChild("Login/RegisterButton"));

		messageBox_ = window_->getChild("Login/MessageBox");
		messageBox_->hide();
		messageBox_->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&LoginWindow::HandleMessageBoxKeyPress, this));

		AnimationInstance * instance = AnimationManager::getSingletonPtr()->instantiateAnimation("LoginTitle");
		instance->setTargetWindow(window_->getChild("Login/Title"));
		instance->start();

		System::getSingleton().setGUISheet(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "LoginWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

LoginWindow::~LoginWindow()
{
	AnimationManager::getSingletonPtr()->destroyAnimation("LoginTitle");
	window_->destroy();
}

void LoginWindow::SetOKButtonEventHandler(Event::Subscriber subscriber)
{
	okButton_->subscribeEvent(PushButton::EventClicked, subscriber);
}

void LoginWindow::SetCancelButtonEventHandler(Event::Subscriber subscriber)
{
	cancelButton_->subscribeEvent(PushButton::EventClicked, subscriber);
}

void LoginWindow::SetAccountButtonEventHandler(Event::Subscriber subscriber)
{
	accountButton_->subscribeEvent(PushButton::EventClicked, subscriber);
}

void LoginWindow::SetRegisterButtonEventHandler(Event::Subscriber subscriber)
{
	registerButton_->subscribeEvent(PushButton::EventClicked, subscriber);
}

void LoginWindow::ShowMessageText(const std::string& text)
{
	messageBox_->getChild("Login/MessageBox/Text")->setText(text);
	messageBox_->setModalState(true);
	messageBox_->show();
	messageBox_->activate();
}

void LoginWindow::HideMessageText()
{
	messageBox_->getChild("Login/MessageBox/Text")->setText("");
	messageBox_->hide();
}

const std::string LoginWindow::GetUsernameText() const
{
	return usernameEditbox_->getText().c_str();
}

const std::string LoginWindow::GetPasswordText() const
{
	return passwordEditbox_->getText().c_str();
}

bool LoginWindow::HandleKeyPress(const EventArgs& e)
{
	const KeyEventArgs keyArgs = static_cast<const KeyEventArgs&>(e);
	switch (keyArgs.scancode)
	{
	case Key::Return:
		okButton_->fireEvent(PushButton::EventClicked, EventArgs());
		return true;
	case Key::Tab:
		if (usernameEditbox_->isActive())
		{
			passwordEditbox_->activate();
		}
		else
		{
			usernameEditbox_->activate();
		}
		return true;
	}
	return false;
}

bool LoginWindow::HandleMessageBoxKeyPress(const EventArgs& e)
{
	const KeyEventArgs keyArgs = static_cast<const KeyEventArgs&>(e);
	switch (keyArgs.scancode)
	{
	case Key::Escape:
	case Key::Return:
		messageBox_->setModalState(false);
		messageBox_->hide();
		usernameEditbox_->activate();
		return true;
	}
	return false;
}

} // namespace GUI