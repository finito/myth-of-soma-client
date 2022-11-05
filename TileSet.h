#ifndef TILE_SET_H
#define TILE_SET_H

#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <boost/shared_ptr.hpp>

namespace Framework
{
	class TextureAtlasResource;
}

class TileSet
{
public:
	bool Load(FILE& input);

	bool GetSprite(unsigned int subTileSet, unsigned int tile, sf::Sprite& result, sf::Texture*& paletteTexture) const;

private:
	void ReadSubTileSets(FILE& input);

	static const int tileSize_ = 32;

	struct SubTileSet
	{
		int	width;	// Width in tiles
		int	height; // Height in tiles
		std::vector<sf::Vector2i> tiles; // A sub tile set is made up of tiles taken from the main tile set.
		// So if there was 2 tiles in the sub tile set then sourceTiles would
		// contain 2 entries. E.g. _tiles[0] could be Vector2i of 2,2 and
		// _tiles[1] could be Vector2i of 5,5. This would mean the
		// sub tile set is made up of the tile at position 2,2 and 5,5 taken
		// from the main tile set image.
	};

	std::vector<SubTileSet> subTileSets_;	// A list of sub tile sets (these are frames of the source image)

	mutable boost::shared_ptr<Framework::TextureAtlasResource> textureAtlas_;
	std::string spriteFilename_;
};
#endif // TILE_SET_H