#ifndef TILE_PROPERTIES_H
#define TILE_PROPERTIES_H

#include <cstdio>
#include <vector>
#include <SFML/System/Vector2.hpp>

class TileProperties
{
public:
	void Load(const std::string& filename);

	bool GetMovable(const sf::Vector2i& cell) const;
	int GetEvent(const sf::Vector2i& cell) const;
	int GetHouse(const sf::Vector2i& cell) const;
	int GetArea(const sf::Vector2i& cell) const;

	void SetMovable(const sf::Vector2i& cell, bool value);

	int GetWidth() const;

	int GetHeight() const;

private:
	class Properties
	{
	public:
		Properties() :
		move(true),
		event(-1),
		house(-1),
		area(0)
		{
		}

		bool		  move;
		short		  event;
		short		  house;
		unsigned char area;
	};

	int width_;
	int height_;

	static unsigned char bitMask_[8];

	std::vector<Properties> properties_;
};

#endif // TILE_PROPERTIES_H