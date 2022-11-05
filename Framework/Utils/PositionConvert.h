#ifndef UTILS_POSITION_CONVERT_H
#define UTILS_POSITION_CONVERT_H

#include <SFML/System/Vector2.hpp>

namespace Utils
{
	namespace PositionConvert
	{
		sf::Vector2f CellToPixel(const sf::Vector2i& cellPosition);
		sf::Vector2i PixelToCell(const sf::Vector2f& pixelPosition);
	}
}

#endif // UTILS_POSITION_CONVERT_H