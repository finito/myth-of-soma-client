
#include "GUI/ChatHistoryWindow.h"
#include "GUI/Elements/FormattedListboxTextItem.h"

#include "Network/GameProtocol.h"

#include <iostream>

namespace GUI
{

using namespace CEGUI;
using namespace Network::GameProtocol;

ChatHistoryWindow::ChatHistoryWindow()
{
	chatTypes_.resize(MAX_HISTORY_SIZE);
	chatFiltered_.resize(ChatType::LAST);

	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("chathistorywindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&ChatHistoryWindow::HandleShown, this));

		chatList_ = static_cast<Listbox*>(window_->getChild("ChatHistory/List"));

		normalChatCheckbox_ = static_cast<Checkbox*>(window_->getChild("ChatHistory/NormalChatToggleButton"));
		normalChatCheckbox_->setSelected(true);
		normalChatCheckbox_->subscribeEvent(Checkbox::EventCheckStateChanged,
			Event::Subscriber(&ChatHistoryWindow::HandleNormalChatFilter, this));

		shoutChatCheckbox_ = static_cast<Checkbox*>(window_->getChild("ChatHistory/ShoutChatToggleButton"));
		shoutChatCheckbox_->setSelected(true);
		shoutChatCheckbox_->subscribeEvent(Checkbox::EventCheckStateChanged,
			Event::Subscriber(&ChatHistoryWindow::HandleShoutChatFilter, this));

		partyChatCheckbox_ = static_cast<Checkbox*>(window_->getChild("ChatHistory/PartyChatToggleButton"));
		partyChatCheckbox_->setSelected(true);
		partyChatCheckbox_->subscribeEvent(Checkbox::EventCheckStateChanged,
			Event::Subscriber(&ChatHistoryWindow::HandlePartyChatFilter, this));

		zoneChatCheckbox_ = static_cast<Checkbox*>(window_->getChild("ChatHistory/ZoneChatToggleButton"));
		zoneChatCheckbox_->setSelected(true);
		zoneChatCheckbox_->subscribeEvent(Checkbox::EventCheckStateChanged,
			Event::Subscriber(&ChatHistoryWindow::HandleZoneChatFilter, this));

		guildChatCheckbox_ = static_cast<Checkbox*>(window_->getChild("ChatHistory/GuildChatToggleButton"));
		guildChatCheckbox_->setSelected(true);
		guildChatCheckbox_->subscribeEvent(Checkbox::EventCheckStateChanged,
			Event::Subscriber(&ChatHistoryWindow::HandleGuildChatFilter, this));

		worldChatCheckbox_ = static_cast<Checkbox*>(window_->getChild("ChatHistory/WorldChatToggleButton"));
		worldChatCheckbox_->setSelected(true);

		closeButton_ = static_cast<PushButton*>(window_->getChild("ChatHistory/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ChatHistoryWindow::HandleClose, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "ChatHisotryWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

ChatHistoryWindow::~ChatHistoryWindow()
{
	window_->destroy();
}

void ChatHistoryWindow::AddChat(const std::string& chatMessage, const unsigned char chatType, colour chatColor)
{
	if (chatFiltered_[chatType])
	{
		return;
	}

	switch (chatType)
	{
	case ChatType::NORMAL:
		chatColor.set(255 / 255.f, 255 / 255.f, 255 / 255.f);
		break;
	case ChatType::SYSTEM:
		chatColor.set(255 / 255.f, 255 / 255.f, 0 / 255.f);
		break;
	case ChatType::GM_NOTICE:
		chatColor.set(0 / 255.f, 255 / 255.f, 255 / 255.f);
		break;
	case ChatType::GUILD:
		chatColor.set(0 / 255.f, 255 / 255.f, 0 / 255.f);
		break;
	case ChatType::SHOUT:
		chatColor.set(253 / 255.f, 139 / 255.f, 93 / 255.f);
		break;
	case ChatType::YELLOW_STAT:
		chatColor.set(255 / 255.f, 255 / 255.f, 0 / 255.f);
		break;
	case ChatType::BLUE_STAT:
		chatColor.set(0 / 255.f, 255 / 255.f, 255 / 255.f);
		break;
	case ChatType::PARTY:
		chatColor.set(255 / 255.f, 128 / 255.f, 255 / 255.f);
		break;
	}

	// Purge old items one at a time
	while (chatList_->getItemCount() >= MAX_HISTORY_SIZE)
	{
		chatList_->removeItem(chatList_->getListboxItemFromIndex(0));
	}

	FormattedListboxTextItem* chatItem = new FormattedListboxTextItem(chatMessage, HTF_WORDWRAP_LEFT_ALIGNED);
	chatItem->setTextColours(chatColor);
	chatTypes_[chatList_->getItemCount()] = chatType;
	chatItem->setUserData(&chatTypes_[chatList_->getItemCount()]);
	chatList_->addItem(chatItem);
}

bool ChatHistoryWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool ChatHistoryWindow::HandleShown(const EventArgs& e)
{
	if (chatList_->getItemCount() > 0)
	{
		chatList_->ensureItemIsVisible(chatList_->getItemCount()-1);
	}
	return true;
}

void ChatHistoryWindow::FilterChat(const unsigned char chatType, const bool filtered)
{
	chatFiltered_[chatType] = filtered;
}

bool ChatHistoryWindow::HandleNormalChatFilter(const EventArgs& e)
{
	FilterChat(ChatType::NORMAL, !normalChatCheckbox_->isSelected());
	return true;
}

bool ChatHistoryWindow::HandleShoutChatFilter(const EventArgs& e)
{
	FilterChat(ChatType::SHOUT, !shoutChatCheckbox_->isSelected());
	return true;
}

bool ChatHistoryWindow::HandlePartyChatFilter(const EventArgs& e)
{
	FilterChat(ChatType::PARTY, !partyChatCheckbox_->isSelected());
	return true;
}

bool ChatHistoryWindow::HandleZoneChatFilter(const EventArgs& e)
{
	FilterChat(ChatType::ZONE, !zoneChatCheckbox_->isSelected());
	return true;
}

bool ChatHistoryWindow::HandleGuildChatFilter(const EventArgs& e)
{
	FilterChat(ChatType::GUILD, !guildChatCheckbox_->isSelected());
	return true;
}

} // namespace GUI