#ifndef GUI_INPUT_BOX_WINDOW_H
#define GUI_INPUT_BOX_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

class InputBoxWindow
{
public:
    static CEGUI::String EventInputBoxResult;

	InputBoxWindow();
	~InputBoxWindow();

	void Show(const std::string& title, CEGUI::Event::Subscriber resultHandler);

	void SetInputText(const CEGUI::String value);
	const CEGUI::String GetInputText() const;

	void SetUserData(void* value);
	void* GetUserData() const;

private:
	void Hide(bool fireResult = true);

	bool HandleOKButtonClicked(const CEGUI::EventArgs& e);

	bool HandleKeyPress(const CEGUI::EventArgs& e);

private:
	CEGUI::Window*     window_;
	CEGUI::Window*     titleText_;
	CEGUI::Editbox*	   inputEditbox_;
	CEGUI::PushButton* okButton_;
	void* userData_;
};

} // namespace GUI

#endif // INPUT_BOX_WINDOW_H