
#include "TileSet.h"
#include <cstdio>
#include <string>
#include <SFML/Graphics/Rect.hpp>
#include "Framework/Assets/ResourceCache.h"
#include "Framework/Graphics/TextureRegion.h"

bool TileSet::Load(FILE& input)
{
	// Read the header for each Tile Set
	fseek(&input, 64, SEEK_CUR); // Skip Name
	char buffer[64];
	fread(&buffer, sizeof(char), 64, &input);
	fseek(&input, 4, SEEK_CUR); // Skip crCK

	bool result = false;

	std::string spriteFilename = buffer;
	if (!spriteFilename.empty())
	{
		ReadSubTileSets(input);

		spriteFilename_ = spriteFilename.substr(0, spriteFilename.length() - 4) + ".dat";
		result = true;
	}

	return result;
}

void TileSet::ReadSubTileSets(FILE& input)
{
	unsigned int subTileSetSize = 0;
	fread(&subTileSetSize, sizeof(unsigned int), 1, &input);

	// Read each Sub Tile Set
	for (unsigned int i = 0; i < subTileSetSize; ++i)
	{
		SubTileSet sts;

		fseek(&input, 64, SEEK_CUR); // Skip Remark
		fread(&sts.width,  sizeof(int), 1, &input);
		fread(&sts.height, sizeof(int), 1, &input);

		unsigned int aniCount = 1;
		fread(&aniCount, sizeof(int), 1, &input); // Do not think it is used, but need to read it.

		fseek(&input, 4 * sts.width * sts.height, SEEK_CUR); // Skip pnFlags

		// Load the offsets of each tile that is contained in the sub tile set
		for (int j = 0; j < sts.width * sts.height; ++j)
		{
			sf::Vector2i offset;
			fread(&offset.x, sizeof(int), 1, &input);
			fread(&offset.y, sizeof(int), 1, &input);
			sts.tiles.push_back(offset);
		}

		// Ani (Don't think it is used so skipping it all)
		if (aniCount > 1)
		{
			fseek(&input, sizeof(char) * aniCount, SEEK_CUR);
		}

		subTileSets_.push_back(sts);
	}
}

bool TileSet::GetSprite(const unsigned int subTileSet, const unsigned int tile, sf::Sprite& sprite,
	sf::Texture*& paletteTexture) const
{
	if (subTileSet >= subTileSets_.size() || tile >= subTileSets_[subTileSet].tiles.size())
	{
		return false;
	}

	if (textureAtlas_ == nullptr)
	{
		textureAtlas_ = Framework::textureAtlasCache.Get("map/tiles/" + spriteFilename_);
	}

	Framework::TextureRegion* region = textureAtlas_->GetRegion(subTileSet);
	if (region != nullptr)
	{
		const sf::Vector2i firstTileOffset = subTileSets_[subTileSet].tiles[0];
		const sf::Vector2i tileOffset = subTileSets_[subTileSet].tiles[tile];

		sf::IntRect frameSubRect = region->GetTextureRect();
		frameSubRect.left  += (tileOffset.x - firstTileOffset.x) * tileSize_;
		frameSubRect.top   += (tileOffset.y - firstTileOffset.y) * tileSize_;
		frameSubRect.width  = tileSize_;
		frameSubRect.height = tileSize_;

		sprite.setTexture(*region->GetTexture());
		sprite.setTextureRect(frameSubRect);

		paletteTexture = region->GetPaletteTexture();
		return true;
	}

	return false;
}