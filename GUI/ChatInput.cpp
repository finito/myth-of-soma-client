
#include "GUI/ChatInput.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

ChatInput::ChatInput()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("chatinput.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&ChatInput::HandleShown, this));

		chatEditbox_ = static_cast<Editbox*>(window_->getChild("ChatInput/Text"));
		chatEditbox_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&ChatInput::HandleChatKeyPress, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "ChatInput: Failed to initialize: " << e.what() << std::endl;
	}
}

ChatInput::~ChatInput()
{
	window_->destroy();
}

bool ChatInput::HandleChatKeyPress(const EventArgs& e)
{
	const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);
	if (keyArgs.scancode == Key::Escape || keyArgs.scancode == Key::Return)
	{
		if (keyArgs.scancode == Key::Return && !chatEditbox_->getText().empty())
		{
			SendChat();
		}
		chatEditbox_->setText("");
		window_->hide();
		return true;
	}
	return false;
}

bool ChatInput::HandleShown(const EventArgs& e)
{
	chatEditbox_->activate();
	return true;
}

void ChatInput::SendChat()
{
	GamePacket packet(GameProtocol::PKT_CHAT);
	packet << GameProtocol::ChatType::NORMAL;
	packet.AppendString(chatEditbox_->getText().c_str());
	Game::Instance().gameSocket->Send(packet);
}

} // namespace GUI