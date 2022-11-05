#ifndef GUI_EVENT_MESSAGE_WINDOW_H
#define GUI_EVENT_MESSAGE_WINDOW_H

#include <CEGUI/CEGUI.h>
#include <list>

namespace GUI
{

class EventMessageWindow
{
public:
	EventMessageWindow();
	~EventMessageWindow();

	void AddMessage(const std::string& message);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);
	bool HandleMouseClick(const CEGUI::EventArgs& e);

private:
	CEGUI::Window* window_;
	CEGUI::Window* text_;

	std::list<CEGUI::String> messageList_;
};

} // namespace GUI

#endif // GUI_EVENT_MESSAGE_WINDOW_H