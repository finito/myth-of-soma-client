#ifndef MOUSE_CURSOR_SYSTEM_H
#define MOUSE_CURSOR_SYSTEM_H

#include "Artemis/World.h"
#include "Artemis/EntitySystem.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include "Framework/Utils/Enum.h"

namespace MouseCursor
{
	DEFINE_ENUM_WITH_STRING_CONVERSIONS(Enum, (Normal)(Pickup)(Battle)(PK))
}

class MouseCursorSystem : public artemis::EntitySystem
{
public:
	MouseCursorSystem(sf::RenderTarget& renderTarget);

	virtual void initialize();

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*>& entities);

	void SetMouseCursor(MouseCursor::Enum value);

private:
	bool checkProcessing();

	unsigned int GetFrameCount();

	MouseCursor::Enum mouseCursor_;
	float elapsedTime_;
	unsigned int mouseCursorFrame_;
};

#endif // MOUSE_CURSOR_SYSTEM_H