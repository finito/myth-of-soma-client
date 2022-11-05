#ifndef GUI_LOGIN_WINDOW_H
#define GUI_LOGIN_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

class LoginWindow
{
public:
	LoginWindow();
	~LoginWindow();

	void SetOKButtonEventHandler(CEGUI::Event::Subscriber subscriber);
	void SetCancelButtonEventHandler(CEGUI::Event::Subscriber subscriber);
	void SetAccountButtonEventHandler(CEGUI::Event::Subscriber subscriber);
	void SetRegisterButtonEventHandler(CEGUI::Event::Subscriber subscriber);

	//void setKeyPressedHandler(CEGUI::Event::Subscriber subscriber);
	//void setUsernameKeyPressedHandler(CEGUI::Event::Subscriber subscriber);
	//void setPasswordKeyPressedHandler(CEGUI::Event::Subscriber subscriber);

	void ShowMessageText(const std::string& text);
	void HideMessageText();

	const std::string GetUsernameText() const;
	const std::string GetPasswordText() const;

private:
	bool HandleKeyPress(const CEGUI::EventArgs& e);
	bool HandleMessageBoxKeyPress(const CEGUI::EventArgs& e);

private:
	CEGUI::Window*	   window_;
	CEGUI::Editbox*	   usernameEditbox_;
	CEGUI::Editbox*	   passwordEditbox_;
	CEGUI::PushButton* okButton_;
	CEGUI::PushButton* cancelButton_;
	CEGUI::PushButton* accountButton_;
	CEGUI::PushButton* registerButton_;
	CEGUI::Window*     messageBox_;
};

} // namespace GUI

#endif // GUI_LOGIN_WINDOW_H