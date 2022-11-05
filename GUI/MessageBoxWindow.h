#ifndef GUI_MESSAGE_BOX_WINDOW_H
#define GUI_MESSAGE_BOX_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

namespace MessageBoxMode
{
	enum Enum
	{
		OK,
		Cancel,
		OKCancel,
	};
}

namespace MessageBoxResult
{
	enum Enum
	{
		OK,
		Cancel,
	};
}

class MessageBoxWindow
{
public:
    static CEGUI::String EventMessageBoxResult;

	MessageBoxWindow();
	~MessageBoxWindow();

	void Show(const CEGUI::String& message, MessageBoxMode::Enum mode = MessageBoxMode::OK);

	void Show(const CEGUI::String& message, MessageBoxMode::Enum mode, 
		CEGUI::Event::Subscriber resultHandler);

	MessageBoxResult::Enum GetResult() const;

	void Hide(bool fireEvent = true);

private:
	bool HandleOKButtonClicked(const CEGUI::EventArgs& e);
	bool HandleCancelButtonClicked(const CEGUI::EventArgs& e);

	bool HandleKeyPress(const CEGUI::EventArgs& e);

private:
	CEGUI::Window*		   window_;
	CEGUI::Window*		   text_;
	CEGUI::PushButton*	   okButton_;
	CEGUI::PushButton*	   cancelButton_;
	CEGUI::PushButton*	   okCancelOKButton_;
	CEGUI::PushButton*	   okCancelCancelButton_;
	MessageBoxResult::Enum result_;
	MessageBoxMode::Enum   mode_;
};

} // namespace GUI

#endif // GUI_MESSAGE_BOX_WINDOW_H