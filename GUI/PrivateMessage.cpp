
#include "GUI/PrivateMessage.h"
#include "GUI/Elements/FormattedListboxTextItem.h"

#include "Components/AttributeComponent.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

PrivateMessage::PrivateMessage()
{
	try
	{
		dragging_ = false;

		window_ = WindowManager::getSingleton().loadWindowLayout("privatemessagewindow.layout");
		window_->moveToBack();
		window_->getChild("PrivateMessage/MessageIcon")->hide();
		window_->getChild("PrivateMessage/MessageFrame")->hide();

		layout_ = static_cast<VerticalLayoutContainer*>(window_->getChild("PrivateMessage/LayoutContainer"));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "PrivateMessageWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

PrivateMessage::~PrivateMessage()
{
	window_->destroy();
}

void PrivateMessage::AddMessageFrame(const String& name, const unsigned short classType, const bool myMessage)
{
	const String frameWindowName = "PrivateMessage/MessageFrame_" + name;
	Window* messageFrame = window_->getChild("PrivateMessage/MessageFrame")->clone(frameWindowName);
	messageFrame->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&PrivateMessage::HandleMessageFrameMouseClick, this));
	messageFrame->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&PrivateMessage::HandleMessageFrameMouseButtonDown, this));
	messageFrame->subscribeEvent(Window::EventMouseButtonUp, Event::Subscriber(&PrivateMessage::HandleMessageFrameMouseButtonUp, this));
	messageFrame->subscribeEvent(Window::EventMouseMove, Event::Subscriber(&PrivateMessage::HandleMessageFrameMouseMove, this));
	messageFrame->subscribeEvent(Window::EventInputCaptureLost, Event::Subscriber(&PrivateMessage::HandleMessageFrameCaptureLost, this));
	messageFrame->subscribeEvent(Window::EventShown, Event::Subscriber(&PrivateMessage::HandleMessageFrameShown, this));
	messageFrame->subscribeEvent(Window::EventHidden, Event::Subscriber(&PrivateMessage::HandleMessageFrameHidden, this));
	messageFrame->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&PrivateMessage::HandleMessageFrameScroll, this));
	messageFrame->hide();
	messageFrame->setUserString("name", name);

	Editbox* chatEditbox = static_cast<Editbox*>(messageFrame->getChild(frameWindowName + "/ChatInput"));
	chatEditbox->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&PrivateMessage::HandleMessageFrameChatInputKeyPress, this));

	PushButton* closeButton = static_cast<PushButton*>(messageFrame->getChild(frameWindowName + "/CloseButton"));
	closeButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&PrivateMessage::HandleMessageFrameClose, this));
	closeButton->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&PrivateMessage::HandleMessageFrameCloseMouseLeaves, this));

	Window* characterName = messageFrame->getChild(frameWindowName + "/Name");
	characterName->setText(name);
	characterName->show();

	messageFrame->getChild(frameWindowName + "/CloseButton")->show();
	messageFrame->getChild(frameWindowName + "/ChatInput")->show();

	Listbox* chatList = static_cast<Listbox*>(messageFrame->getChild(frameWindowName + "/ChatList"));
	chatList->show();

	window_->addChildWindow(messageFrame);
	if (myMessage)
	{
		messageFrame->setVisible(true);
	}
	else
	{
		messageFrame->setVisible(false);
	}
}

void PrivateMessage::AddMessageIcon(const String& name, const unsigned short classType, const bool myMessage)
{
	const String iconWwindowName = "PrivateMessage/MessageIcon_" + name;
	Window* messageIcon = window_->getChild("PrivateMessage/MessageIcon")->clone(iconWwindowName);
	messageIcon->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&PrivateMessage::HandleMessageIconMouseClick, this));

	Window* iconImage = messageIcon->getChild(iconWwindowName + "/Image");
	Window* characterName = messageIcon->getChild(iconWwindowName + "/Name");

	const Size displaySize = System::getSingleton().getRenderer()->getDisplaySize();

	float totalWidth  = 0.f;
	float totalHeight = 0.f;

	iconImage->setProperty("Image", "set:privatemessagewindow image:MessageIcon");
	iconImage->show();

	characterName->setText(name);
	characterName->show();

	const float characterNameHeight = PropertyHelper::stringToFloat(characterName->getProperty("VertExtent"));
	characterName->setHeight(UDim(0, characterNameHeight));

	const float characterNameWidth = PropertyHelper::stringToFloat(characterName->getProperty("HorzExtent"));		
	characterName->setWidth(UDim(0, characterNameWidth));

	UDim messageIconHeight = messageIcon->getHeight();
	Point position = characterName->getUnclippedOuterRect().getPosition();
	if ((position.d_x + characterNameWidth) > std::abs(layout_->getXPosition().d_offset))
	{
		position.d_x -= (position.d_x + characterNameWidth) - std::abs(layout_->getXPosition().d_offset);
	}
	characterName->setPosition(UVector2(UDim(0, position.d_x), UDim(0, position.d_y + messageIconHeight.d_offset + 2.0f)));

	messageIconHeight.d_offset += characterNameHeight + 2.0f;
	messageIcon->setHeight(messageIconHeight);
	messageIcon->setUserString("name", name);

	layout_->addChildWindow(messageIcon);
	messageIcon->setID(layout_->getPositionOfChildWindow(messageIcon));
	layout_->layout();

	if (myMessage)
	{
		messageIcon->hide();
		window_->addChildWindow(messageIcon);
	}
	else
	{
		messageIcon->show();
	}
}

void PrivateMessage::Add(const String& name, const String& chatMessage, const unsigned short classType, const bool myMessage)
{
	const String frameWindowName = "PrivateMessage/MessageFrame_" + name;
	const String iconWwindowName = "PrivateMessage/MessageIcon_" + name;

	if (!window_->isChild(frameWindowName))
	{
		AddMessageFrame(name, classType, myMessage);
	}

	if (!window_->isChild(iconWwindowName) && !layout_->isChild(iconWwindowName))
	{
		AddMessageIcon(name, classType, myMessage);
	}

	if (window_->isChild(frameWindowName))
	{
		Window* messageFrame = window_->getChild(frameWindowName);
		Listbox* chatList = static_cast<Listbox*>(messageFrame->getChild(frameWindowName + "/ChatList"));

		// Purge old items one at a time
		while (chatList->getItemCount() >= MAX_HISTORY_SIZE)
		{
			chatList->removeItem(chatList->getListboxItemFromIndex(0));
		}

		const String text = myMessage ? "> " + chatMessage : "< " + chatMessage;
		FormattedListboxTextItem* chatItem = new FormattedListboxTextItem(text, HTF_WORDWRAP_LEFT_ALIGNED);
		if (!myMessage)
		{
			chatItem->setTextColours(colour(0 / 255.f, 255 / 255.f, 255 / 255.f));
		}

		bool scrollbarAtBottom = true;
		Scrollbar* scrollBar = chatList->getVertScrollbar();
		if (scrollBar->getScrollPosition() < scrollBar->getDocumentSize() - scrollBar->getPageSize())
		{
			scrollbarAtBottom = false;
		}

		chatList->addItem(chatItem);

		if (scrollbarAtBottom || myMessage)
		{
			chatList->ensureItemIsVisible(chatList->getItemCount()-1);
		}
	}

	if (layout_->isChild(iconWwindowName))
	{
		Window* messageIcon = layout_->getChild(iconWwindowName);
		Window* iconImage = messageIcon->getChild(iconWwindowName + "/Image");
		iconImage->setProperty("Image", "set:privatemessagewindow image:MessageIconNew");
	}
}

bool PrivateMessage::HandleMessageIconMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == RightButton)
	{
		Window* messageIcon = mouseEventArgs.window;
		messageIcon->hide();
		window_->addChildWindow(messageIcon);

		Window* iconImage = mouseEventArgs.window->getChild(messageIcon->getName() + "/Image");
		iconImage->setProperty("Image", "set:privatemessagewindow image:MessageIcon");

		const String frameWindowName = "PrivateMessage/MessageFrame_" + messageIcon->getUserString("name");
		if (window_->isChild(frameWindowName))
		{
			Window* messageFrame = window_->getChild(frameWindowName);
			float imageWidth = ImagesetManager::getSingleton().get("privatemessagewindow").getImage("PrivateMessageWindow").getWidth();
			messageFrame->setXPosition(UDim(1.0, -imageWidth));
			messageFrame->setYPosition(messageIcon->getYPosition());
			messageFrame->show();
		}
	}
	return true;
}

bool PrivateMessage::HandleMessageFrameMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == RightButton)
	{
		Window* messageFrame = mouseEventArgs.window;
		messageFrame->hide();
	}
	return true;
}

bool PrivateMessage::HandleMessageFrameMouseButtonDown(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		if (mouseEventArgs.window->captureInput())
		{
			dragging_ = true;
			dragPoint_ = CoordConverter::screenToWindow(*mouseEventArgs.window, mouseEventArgs.position);
			oldCursorArea_ = MouseCursor::getSingleton().getConstraintArea();

			Rect constrainArea;
			Rect screen(Vector2(0, 0), System::getSingleton().getRenderer()->getDisplaySize());
			constrainArea = screen.getIntersection(oldCursorArea_);
			MouseCursor::getSingleton().setConstraintArea(&constrainArea);
		}
	}
	return true;
}

bool PrivateMessage::HandleMessageFrameMouseButtonUp(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		mouseEventArgs.window->releaseInput();
	}
	return true;
}

bool PrivateMessage::HandleMessageFrameMouseMove(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (dragging_)
	{
		Vector2 delta(CoordConverter::screenToWindow(*mouseEventArgs.window, mouseEventArgs.position));
		delta -= dragPoint_;
		UVector2 offset(cegui_absdim(delta.d_x), cegui_absdim(delta.d_y));
		mouseEventArgs.window->setPosition(mouseEventArgs.window->getArea().getPosition() + offset);
	}
	return true;
}

bool PrivateMessage::HandleMessageFrameCaptureLost(const EventArgs& e)
{
	dragging_ = false;
	MouseCursor::getSingleton().setConstraintArea(&oldCursorArea_);
	return true;
}

bool PrivateMessage::HandleMessageFrameShown(const EventArgs& e)
{
	const WindowEventArgs& windowEventArgs = static_cast<const WindowEventArgs&>(e);
	windowEventArgs.window->getChild(windowEventArgs.window->getName() + "/ChatInput")->activate();
	Listbox* chatList = static_cast<Listbox*>(windowEventArgs.window->getChild(windowEventArgs.window->getName() + "/ChatList"));
	chatList->ensureItemIsVisible(chatList->getItemCount()-1);
	return true;
}

bool PrivateMessage::HandleMessageFrameHidden(const EventArgs& e)
{
	const WindowEventArgs& windowEventArgs = static_cast<const WindowEventArgs&>(e);
	const String iconWindowName = "PrivateMessage/MessageIcon_" + windowEventArgs.window->getUserString("name");
	if (window_->isChild(iconWindowName))
	{
		Window* messageIcon = window_->getChild(iconWindowName);
		messageIcon->show();
		layout_->addChildWindowToPosition(messageIcon, messageIcon->getID());
	}
	return true;
}

bool PrivateMessage::HandleMessageFrameChatInputKeyPress(const EventArgs& e)
{
	const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);
	switch (keyArgs.scancode)
	{
	case Key::Return:
		if (!keyArgs.window->getText().empty())
		{
			SendChat(keyArgs.window->getParent()->getUserString("name"), keyArgs.window->getText());
		}
		keyArgs.window->setText("");
		return true;
	case Key::Escape:
		keyArgs.window->getParent()->hide();
		return true;
	case Key::ArrowUp: // Fall-through!
	case Key::PageUp:
		{
		Listbox* chatList = static_cast<Listbox*>(keyArgs.window->getParent()->getChild(keyArgs.window->getParent()->getName() + "/ChatList"));
		chatList->getVertScrollbar()->setScrollPosition(chatList->getVertScrollbar()->getScrollPosition() + chatList->getVertScrollbar()->getStepSize());
		}
		return true;
	case Key::ArrowDown: // Fall-through!
	case Key::PageDown:
		{
		Listbox* chatList = static_cast<Listbox*>(keyArgs.window->getParent()->getChild(keyArgs.window->getParent()->getName() + "/ChatList"));
		chatList->getVertScrollbar()->setScrollPosition(chatList->getVertScrollbar()->getScrollPosition() - chatList->getVertScrollbar()->getStepSize());
		}
		return true;
	}
	return false;
}

bool PrivateMessage::HandleMessageFrameScroll(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	Window* messageFrame = mouseEventArgs.window;
	Scrollbar* scrollBar = static_cast<Listbox*>(messageFrame->getChild(messageFrame->getName() + "/ChatList"))->getVertScrollbar();

	float scrollAmount = 0;
	if (mouseEventArgs.wheelChange < 0)
	{
		scrollAmount = scrollBar->getStepSize();
	}
	else if (mouseEventArgs.wheelChange > 0)
	{
		scrollAmount = -scrollBar->getStepSize();
	}

	scrollBar->setScrollPosition(scrollBar->getScrollPosition() + scrollAmount);
	return true;
}

bool PrivateMessage::HandleMessageFrameClose(const EventArgs& e)
{
	const WindowEventArgs& windowEventArgs = static_cast<const WindowEventArgs&>(e);
	Window* messageFrame = windowEventArgs.window->getParent();
	const String iconWindowName = "PrivateMessage/MessageIcon_" + messageFrame->getUserString("name");
	if (window_->isChild(iconWindowName))
	{
		window_->getChild(iconWindowName)->destroy();
	}
	messageFrame->destroy();
	return true;
}

bool PrivateMessage::HandleMessageFrameCloseMouseLeaves(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	Window* messageFrame = mouseEventArgs.window->getParent();
	messageFrame->getChild(messageFrame->getName() + "/ChatInput")->activate();
	return true;
}

void PrivateMessage::SendChat(const String& name, const String& text)
{
	GamePacket packet(GameProtocol::PKT_CHAT);
	packet << GameProtocol::ChatType::WHISPER << name.c_str();
	packet.AppendString(text.c_str());
	Game::Instance().gameSocket->Send(packet);
}

} // namespace GUI