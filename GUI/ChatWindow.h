#ifndef GUI_CHAT_WINDOW_H
#define GUI_CHAT_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

class ChatWindow
{
public:
	static CEGUI::String EventChatAdded;

	ChatWindow();
	~ChatWindow();

private:
	void UpdateChat(bool addingNewChat);
	void RemoveOldChat();

	bool HandleWindowUpdated(const CEGUI::EventArgs& e);
	bool HandleChatHistoryShown(const CEGUI::EventArgs& e);
	bool HandleChatHistoryHidden(const CEGUI::EventArgs& e);
	bool HandleChatAdded(const CEGUI::EventArgs& e);

private:
	static const int MAX_SIZE = 5;
	static const float chatRenderDuration_;

	CEGUI::Window*	window_;
	CEGUI::Listbox* chatList_;
	CEGUI::Listbox* chatHistoryList_; // This is where will get the items to fill the chat list with
	float			elapsedTime_;
	size_t			chatHistoryIndex_;
};

} // namespace GUI

#endif // GUI_CHAT_WINDOW_H