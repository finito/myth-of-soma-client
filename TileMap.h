#ifndef TILE_MAP_H
#define TILE_MAP_H

#include <string>
#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include "TileSet.h"

// Forward declaration
//class TileProperties;

class TileMap
{
public:
	TileMap();

	// This will load the TMN, TSD and IMF files.
	// TMN is the Tile Map, TSD is Tile Sets and IMF is Tile Properties.
	void Load(const std::string& name);

	bool GetTileSprite(const unsigned int layer, const unsigned int x, const unsigned int y, sf::Sprite& sprite,
		sf::Texture*& paletteTexture) const;

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

private:
	static const unsigned int blockWidth_	   = 25;  // Only used during loading
	static const unsigned int blockHeight_	   = 19;  // Only used during loading
	static const unsigned int maxTileMapLayer_ = 6;	  // TODO: The last layer might not be used. Check this.
	static const unsigned int maxTileSet_      = 100;

	struct Tile
	{
		// Sets unsigned int bit field to max uint -1.
		// The value is used to identify a empty tile when loading the tile map.
		Tile() : tileSet(-1)
		{
		}

		unsigned int tileSet	: 10; // Index of TileSet within the TileMap.
		unsigned int subTileSet : 6;  // Index for a SubTileSet within TileSet.
		unsigned int tile		: 12; // Index to tile within the SubTileSet.
		unsigned int flag		: 4;  // Need to check if this is used anywhere or if it is just padding.
	};

	void LoadTileMap(const std::string& filename);
	void LoadTileSets(const std::string& filename);
	void LoadTileProperties(const std::string& filename);

	unsigned int width_;  // Width of the tile map in tiles
	unsigned int height_; // Height of the tile map in tiles

	std::vector<Tile> tiles_[maxTileMapLayer_]; // This could be moved to a Layer class
	std::vector<TileSet> tileSets_;
};

#endif // TILE_MAP_H