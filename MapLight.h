#ifndef MAP_LIGHT_H
#define MAP_LIGHT_H

#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include "Framework/Graphics/Light.h"

class MapLight
{
public:
	void Load(const std::string& filename);

	std::vector<Framework::Light>& GetLights();

	Framework::Light* AddLight(const sf::Vector2f& position, const sf::Vector2f& scale, const sf::Color& color);

private:
	std::vector<Framework::Light> lights_;
};

#endif // MAP_LIGHT_H