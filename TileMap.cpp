
#include "TileMap.h"
#include <SFML/Graphics.hpp>
#include <cassert>

TileMap::TileMap() :
width_(0),
height_(0)
{
}

void TileMap::Load(const std::string& name)
{
	LoadTileMap	(name + ".tmn");
	LoadTileSets(name + ".tsd");
}

void TileMap::LoadTileMap(const std::string& filename)
{
	FILE* file = fopen(filename.c_str(), "rb");

	assert(file != nullptr);

	if (file == 0)
	{
		// TODO: Add error output here
		return;
	}

	fseek(file, 68, SEEK_CUR); // Skips ID and Remark
	fread(&width_, sizeof(width_), 1, file);
	fread(&height_, sizeof(height_), 1, file);
	fseek(file, 436, SEEK_CUR); // Skips szBMPFNs, crCKs, nResrcQts and FileSize

	// Initialize Map Data
	for (unsigned int i = 0; i < maxTileMapLayer_; ++i)
	{
		tiles_[i].clear();
		tiles_[i].shrink_to_fit();
		tiles_[i].resize(width_ * height_);
	}

	// Calculate amount of blocks of map data there should be
	unsigned int blockX = static_cast<int>(
		ceil(static_cast<double>(width_)  / blockWidth_));
	unsigned int blockY = static_cast<int>(
		ceil(static_cast<double>(height_) / blockHeight_));
	unsigned int blocks = (blockX * blockY);

	// Skip file offsets for the block data
	fseek(file, sizeof(int)*blocks, SEEK_CUR);

	for (unsigned int block = 0; block < blocks; ++block)
	{
		unsigned int minX = (block % blockX) * blockWidth_;
		unsigned int minY = (block / blockX) * blockHeight_;
		unsigned int maxX = minX + blockWidth_;
		unsigned int maxY = minY + blockHeight_;

		for (unsigned int layer = 0; layer < maxTileMapLayer_; ++layer)
		{
			unsigned int x = minX;
			unsigned int y = minY;
			int read = 0;
			fread(&read, sizeof(int), 1, file);
			if (read == 0)
			{
				// Skips entire Layer (Really??)
				fseek(file, 4, SEEK_CUR);
			}
			else
			{
				// Amount of "Gaps" in the current block layer
				for (int r = 0; r < read; ++r)
				{
					int skip = 0;
					fread(&skip, sizeof(int), 1, file);

					int tiles = 0;
					fread(&tiles, sizeof(int), 1, file);

					// Tiles to skip (No tile is there (empty))
					for (int s = 0; s < skip; ++s)
					{
						++x;
						if (x >= maxX)
						{
							x = minX;
							++y;
						}
					}

					// Tiles to read from file (Not Empty)
					for (int t = 0; t < tiles; ++t)
					{
						unsigned int tmd_d = -1;
						fread(&tmd_d, sizeof(unsigned int), 1, file);
						if (static_cast<int>(tmd_d) != -1)
						{
							Tile& t = tiles_[layer][y*width_+x];
							memcpy(&t, &tmd_d, sizeof(unsigned int));
						}
						++x;
						if(x >= maxX)
						{
							x = minX;
							++y;
						}
					}
				}
			}
		}
	}

	fclose(file);
}

void TileMap::LoadTileSets(const std::string& filename)
{
	FILE* file = fopen(filename.c_str(), "rb");
	assert(file != nullptr);
	if (file == nullptr)
	{
		// TODO: Add error output here
		return;
	}

	tileSets_.clear();
	tileSets_.shrink_to_fit();
	tileSets_.resize(maxTileSet_);

	std::for_each(tileSets_.begin(), tileSets_.end(), [&](TileSet& v) { v.Load(*file); });

	fclose(file);
}

bool TileMap::GetTileSprite(const unsigned int layer, const unsigned int x, const unsigned int y, sf::Sprite& sprite,
	sf::Texture*& paletteTexture) const
{
	if (layer > 3 || x < 0 || x >= width_ || y < 0 || y >= height_)
	{
		return false;
	}

	const Tile& t = tiles_[layer][y * width_ + x];
	if (t.tileSet >= tileSets_.size())
	{
		return false;
	}

	return tileSets_[t.tileSet].GetSprite(t.subTileSet, t.tile, sprite, paletteTexture);
}

unsigned int TileMap::GetWidth() const
{
	return width_;
}

unsigned int TileMap::GetHeight() const
{
	return height_;
}