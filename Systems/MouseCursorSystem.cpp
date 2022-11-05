
#include "Systems/MouseCursorSystem.h"

#include <CEGUI/CEGUIMouseCursor.h>
#include <CEGUI/CEGUIPropertyHelper.h>

using namespace artemis;

MouseCursorSystem::MouseCursorSystem(sf::RenderTarget& renderTarget) :
mouseCursor_(MouseCursor::Normal),
mouseCursorFrame_(1),
elapsedTime_(0)
{

}

void MouseCursorSystem::initialize()
{

}

void MouseCursorSystem::processEntities(artemis::ImmutableBag<artemis::Entity*>& entities)
{
	elapsedTime_ += world->getDelta();
	if (elapsedTime_ >= 0.125f)
	{
		++mouseCursorFrame_;
		if (mouseCursorFrame_ > GetFrameCount())
		{
			mouseCursorFrame_ = 1;
		}
		CEGUI::MouseCursor::getSingleton().setImage("cursors",
			MouseCursor::ToString(mouseCursor_) + CEGUI::PropertyHelper::intToString(mouseCursorFrame_));
		elapsedTime_ -= 0.125f;
	}
}

bool MouseCursorSystem::checkProcessing()
{
	return true;
}

unsigned int MouseCursorSystem::GetFrameCount()
{
	switch (mouseCursor_)
	{
	case MouseCursor::Normal:
		return 1;
	case MouseCursor::Pickup:
		return 12;
	case MouseCursor::Battle:
		return 7;
	case MouseCursor::PK:
		return 7;
	default:
		return 1;
	}
}

void MouseCursorSystem::SetMouseCursor(MouseCursor::Enum value)
{
	mouseCursor_ = value;
}