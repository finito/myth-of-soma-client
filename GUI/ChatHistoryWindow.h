#ifndef GUI_CHAT_HISTORY_WINDOW_H
#define GUI_CHAT_HISTORY_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

class ChatHistoryWindow
{
public:
	static const int MAX_HISTORY_SIZE = 1000;

	ChatHistoryWindow();
	~ChatHistoryWindow();

	void AddChat(const std::string& chatMessage, unsigned char chatType, CEGUI::colour chatColor);

private:
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleShown(const CEGUI::EventArgs& e);

	bool HandleNormalChatFilter(const CEGUI::EventArgs& e);
	bool HandleShoutChatFilter(const CEGUI::EventArgs& e);
	bool HandlePartyChatFilter(const CEGUI::EventArgs& e);
	bool HandleZoneChatFilter(const CEGUI::EventArgs& e);
	bool HandleGuildChatFilter(const CEGUI::EventArgs& e);

	void FilterChat(unsigned char chatType, bool filtered);

private:
	CEGUI::Window*	   window_;
	CEGUI::Listbox*	   chatList_;
	CEGUI::PushButton* closeButton_;
	CEGUI::Checkbox*   normalChatCheckbox_;
	CEGUI::Checkbox*   shoutChatCheckbox_;
	CEGUI::Checkbox*   partyChatCheckbox_;
	CEGUI::Checkbox*   zoneChatCheckbox_;
	CEGUI::Checkbox*   guildChatCheckbox_;
	CEGUI::Checkbox*   worldChatCheckbox_;

	std::vector<unsigned char> chatTypes_;
	std::vector<bool>	       chatFiltered_;
};

} // namespace GUI

#endif // GUI_CHAT_HISTORY_WINDOW_H