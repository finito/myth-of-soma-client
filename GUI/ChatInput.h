#ifndef GUI_CHAT_INPUT_H
#define GUI_CHAT_INPUT_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

class ChatInput
{
public:
	ChatInput();
	~ChatInput();

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleChatKeyPress(const CEGUI::EventArgs& e);

	void SendChat();

private:
	CEGUI::Window*	window_;
	CEGUI::Editbox* chatEditbox_;
};

} // namespace GUI

#endif // GUI_CHAT_INPUT_H