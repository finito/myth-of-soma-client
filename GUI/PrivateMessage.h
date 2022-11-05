#ifndef GUI_PRIVATE_MESSAGE_H
#define GUI_PRIVATE_MESSAGE_H

#include <CEGUI/CEGUI.h>
#include "Artemis/Entity.h"
#include <SFML/System/Vector2.hpp>

namespace GUI
{

class PrivateMessage
{
public:
	PrivateMessage();
	~PrivateMessage();

	void Add(const CEGUI::String& name, const CEGUI::String& chatMessage, unsigned short classType, bool myMessage);

private:
	void AddMessageFrame(const CEGUI::String& name, const unsigned short classType, bool myMessage);
	void AddMessageIcon(const CEGUI::String& name, const unsigned short classType, bool myMessage);


	bool HandleMessageIconMouseClick(const CEGUI::EventArgs& e);

	bool HandleMessageFrameMouseClick(const CEGUI::EventArgs& e);
	bool HandleMessageFrameMouseButtonDown(const CEGUI::EventArgs& e);
	bool HandleMessageFrameMouseButtonUp(const CEGUI::EventArgs& e);
	bool HandleMessageFrameMouseMove(const CEGUI::EventArgs& e);
	bool HandleMessageFrameCaptureLost(const CEGUI::EventArgs& e);
	bool HandleMessageFrameShown(const CEGUI::EventArgs& e);
	bool HandleMessageFrameHidden(const CEGUI::EventArgs& e);
	bool HandleMessageFrameChatInputKeyPress(const CEGUI::EventArgs& e);
	bool HandleMessageFrameScroll(const CEGUI::EventArgs& e);
	bool HandleMessageFrameClose(const CEGUI::EventArgs& e);
	bool HandleMessageFrameCloseMouseLeaves(const CEGUI::EventArgs& e);

	void SendChat(const CEGUI::String& name, const CEGUI::String& text);

private:
	static const int MAX_HISTORY_SIZE = 1000;

	CEGUI::Window* window_;
	CEGUI::VerticalLayoutContainer* layout_;
	CEGUI::Rect oldCursorArea_;
	CEGUI::Point dragPoint_;
	bool dragging_;
};

} // namespace GUI

#endif // GUI_PRIVATE_MESSAGE_H