
#include "GUI/ChatWindow.h"
#include "GUI/ChatHistoryWindow.h"

#include "Elements/FormattedListboxTextItem.h"

#include "Network/GameProtocol.h"

#include <iostream>

namespace GUI
{

using namespace CEGUI;
using namespace Network;

const float ChatWindow::chatRenderDuration_ = 5.f;

String ChatWindow::EventChatAdded = "EventChatAdded";

ChatWindow::ChatWindow() :
elapsedTime_(0),
chatHistoryIndex_(0)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("chatwindow.layout");
		window_->addEvent(EventChatAdded);
		window_->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&ChatWindow::HandleWindowUpdated, this));
		window_->subscribeEvent(EventChatAdded, Event::Subscriber(&ChatWindow::HandleChatAdded, this));

		Window* chatHistory = System::getSingleton().getGUISheet()->getChild("ChatHistory/Frame");
		chatHistory->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&ChatWindow::HandleWindowUpdated, this));
		chatHistory->subscribeEvent(Window::EventShown, Event::Subscriber(&ChatWindow::HandleChatHistoryShown, this));
		chatHistory->subscribeEvent(Window::EventHidden, Event::Subscriber(&ChatWindow::HandleChatHistoryHidden, this));

		chatList_ = static_cast<Listbox*>(window_->getChild("Chat/List"));

		chatHistoryList_ = static_cast<Listbox*>(chatHistory->getChild("ChatHistory/List"));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "ChatWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

ChatWindow::~ChatWindow()
{
	window_->destroy();
}

bool ChatWindow::HandleWindowUpdated(const EventArgs& e)
{
	elapsedTime_ += static_cast<const UpdateEventArgs&>(e).d_timeSinceLastFrame;	
	if (elapsedTime_ >= chatRenderDuration_)
	{
		RemoveOldChat();
		UpdateChat(false);
		elapsedTime_ -= chatRenderDuration_;
	}
	return true;
}

void ChatWindow::RemoveOldChat()
{
	if (chatList_->getItemCount() > 0)
	{
		chatList_->removeItem(chatList_->getListboxItemFromIndex(0));
	}
}

void ChatWindow::UpdateChat(const bool addingNewChat)
{
	using namespace Network::GameProtocol;

	if (addingNewChat)
	{
		elapsedTime_ = 0;
		if (chatHistoryList_->getItemCount() >= ChatHistoryWindow::MAX_HISTORY_SIZE)
		{
			chatHistoryIndex_ = std::max(0u, (chatHistoryIndex_ - 1u));
		}

		if (chatList_->getItemCount() >= MAX_SIZE)
		{
			RemoveOldChat();
		}
	}

	for (size_t i = chatHistoryIndex_; i < chatHistoryList_->getItemCount(); ++i)
	{
		if (chatList_->getItemCount() >= MAX_SIZE)
		{
			break;
		}

		FormattedListboxTextItem* chatItem = static_cast<FormattedListboxTextItem*>(chatHistoryList_->getListboxItemFromIndex(i));
		const unsigned char chatType = *static_cast<unsigned char*>(chatItem->getUserData());
		if (chatType != ChatType::NORMAL)
		{
			FormattedListboxTextItem* newChatItem = new FormattedListboxTextItem(chatItem->getText(), HTF_WORDWRAP_LEFT_ALIGNED);
			newChatItem->setTextColours(chatItem->getTextColours());
			chatList_->addItem(newChatItem);
			chatHistoryIndex_ = i + 1;
		}
	}

	if (addingNewChat)
	{
		if (chatHistoryList_->getItemCount() < ChatHistoryWindow::MAX_HISTORY_SIZE)
		{
			if (chatHistoryList_->getItemCount() + 1 >= ChatHistoryWindow::MAX_HISTORY_SIZE)
			{
				++chatHistoryIndex_;
			}
		}
	}
}

bool ChatWindow::HandleChatHistoryShown(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool ChatWindow::HandleChatHistoryHidden(const EventArgs& e)
{
	window_->show();
	return true;
}

bool ChatWindow::HandleChatAdded(const EventArgs& e)
{
	UpdateChat(true);
	return true;
}

} // namespace GUI