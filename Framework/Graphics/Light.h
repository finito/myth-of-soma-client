#ifndef LIGHT_H
#define LIGHT_H

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace Framework
{

class Light
{
public:
	Light()
	{
	}

	Light(sf::Vector2f position, sf::Vector2f scale, sf::Color color) :
	position(position),
	scale(scale),
	color(color)
	{
	}

	sf::Vector2f position;
	sf::Vector2f scale;
	sf::Color	 color;
};

} // namespace Framework

#endif // LIGHT_H