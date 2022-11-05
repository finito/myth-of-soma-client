#ifndef COMPONENTS_POSITION_COMPONENT_H
#define COMPONENTS_POSITION_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <SFML/System/Vector2.hpp>

class PositionComponent : public artemis::Component
{
public:
	explicit PositionComponent(const sf::Vector2f& position);
	explicit PositionComponent(const sf::Vector2i& cellPosition);

	sf::Vector2f GetPosition();
	void SetPosition(const sf::Vector2f& value);

	sf::Vector2i GetCellPosition();
	void SetCellPosition(const sf::Vector2i& value);

private:
	sf::Vector2f position_;
	sf::Vector2i cellPosition_;
};

#endif // COMPONENTS_POSITION_COMPONENT_H