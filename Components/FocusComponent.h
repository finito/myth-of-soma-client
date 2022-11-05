#ifndef COMPONENTS_FOCUS_COMPONENT_H
#define COMPONENTS_FOCUS_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <SFML/System/Vector2.hpp>
#include <string>

class FocusComponent : public artemis::Component
{
public:
	explicit FocusComponent(const std::string& type);

	sf::Vector2f GetPosition();
	void SetPosition(const sf::Vector2f& value);

	std::string GetType() const;

private:
	std::string type_;
};

#endif // COMPONENTS_FOCUS_COMPONENT_H