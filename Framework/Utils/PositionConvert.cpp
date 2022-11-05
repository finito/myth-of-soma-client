
#include "PositionConvert.h"
#include <cmath>

namespace Utils
{
	namespace PositionConvert
	{
		sf::Vector2f PositionConvert::CellToPixel(const sf::Vector2i& value)
		{
			return sf::Vector2f((value.x + 1) * 48.f, (value.y + 1) * 24.f);
		}

		sf::Vector2i PositionConvert::PixelToCell(const sf::Vector2f& value)
		{
			sf::Vector2i p(static_cast<int>(value.x), static_cast<int>(value.y));

			int X = 48;
			int Y = 24;

			int k1, k2;
			k1 = (int)std::ceil((.5*p.x+p.y-Y)/X);
			k2 = (int)std::ceil((-.5*p.x+p.y+Y)/X);
	
			k1 = X*k1 + Y;
			k2 = X*k2 - Y;
	
			p.x = k1 - k2;
			p.y = (k1+k2)/2-Y;
	
			p.x = int(p.x/24.+.5)*Y;
			p.y = int(p.y/24.+.5)*Y;
	
			p.x /= X;
			p.y /= Y;
			p.x--;
			p.y--;

			if (p.x < 0) p.x = 0;
			if (p.y < 0) p.y = 0;

			return p;
		}
	}
}