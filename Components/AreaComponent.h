#ifndef COMPONENTS_AREA_COMPONENT_H
#define COMPONENTS_AREA_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>

class AreaComponent : public artemis::Component
{
public:
	sf::IntRect GetArea();
	void SetArea(sf::IntRect value);

	bool IsTransparentPixel(const sf::Vector2i& position) const;
	void SetTransparentPixels(const std::vector<bool>* value);

private:
	sf::IntRect area_;
	const std::vector<bool>* transparentPixels_;
};

#endif // COMPONENTS_AREA_COMPONENT_H