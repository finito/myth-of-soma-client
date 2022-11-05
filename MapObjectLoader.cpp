
#include "MapObjectLoader.h"

#include "Components/AreaComponent.h"
#include "Components/PositionComponent.h"

#include "Framework/Assets/ResourceCache.h"
#include "Framework/Graphics/SpriteBatch.h"
#include "Framework/Utils/PositionConvert.h"

#include <boost/algorithm/string/case_conv.hpp>

#include "Zone.h"
#include "Game.h"

using namespace Framework;

MapObjectLoader::MapObjectLoader(Zone& zone) :
width_(0),
height_(0),
zone_(zone),
houseIndex_(-1),
playerAreaComponent_(nullptr),
playerPositionComponent_(nullptr)
{
}

void MapObjectLoader::Load(const std::string& filename)
{
	// TODO: Clear existing loaded map object data
	FILE* file = fopen(filename.c_str(), "rb");

	assert(file != nullptr);

	if (file == 0)
	{
		// TODO: Add error output here
		return;
	}

	fseek(file, 4, SEEK_CUR); // SKIP

	size_t total = 0;
	for (int type = MapObjectType::First; type < MapObjectType::Last; ++type)
	{
		// Skip ID and Remark
		fseek(file, 68, SEEK_CUR); 

		fread(&width_, sizeof(width_), 1, file);
		fread(&height_, sizeof(height_), 1, file);

		// Read ANI filenames
		aniFilenames_[type].clear();
		aniFilenames_[type].shrink_to_fit();
		aniFilenames_[type].reserve(maxSprite);
		for (unsigned int i = 0; i < maxSprite; ++i)
		{
			char buffer[spriteFilenameLength];
			fread(&buffer, sizeof(char), spriteFilenameLength, file);
			boost::to_lower(buffer);
			aniFilenames_[type].push_back(buffer);
		}

		spriteFilenames_[type].clear();
		spriteFilenames_[type].shrink_to_fit();
		spriteFilenames_[type].reserve(maxSprite);
		for (unsigned int i = 0; i < maxSprite; ++i)
		{
			char buffer[spriteFilenameLength];
			fread(&buffer, sizeof(char), spriteFilenameLength, file);
			boost::to_lower(buffer);
			spriteFilenames_[type].push_back(buffer);
			spriteFilenames_[type][i] = spriteFilenames_[type][i].substr(0, spriteFilenames_[type][i].length() - 4) + ".dat";
		}

		// Initialize value array
		spriteValues_[type].clear();
		spriteValues_[type].shrink_to_fit();
		spriteValues_[type].resize(width_ * height_);

		unsigned int count = 0;
		fread(&count, sizeof(unsigned int), 1, file);
		for (unsigned int i = 0; i < count; ++i)
		{
			// Key is Y_Pos * MapWidth + X_Pos
			int key;
			fread(&key, sizeof(int), 1, file);

			// Includes index to the sprite filename
			// and the animation index for the object
			// (there are multiple objects within each sprite which are separated by animation).
			boost::shared_ptr<ObjectData> data = boost::make_shared<ObjectData>();
			data->value = 0;
			if (type == MapObjectType::HouseIn)
			{
				data->alpha = 0;
				data->fadeType = FadeType::Out;
			}
			else
			{
				data->alpha = 255;
				data->fadeType = FadeType::In;
			}
			fread(&data->value, sizeof(short), 1, file);
			fseek(file, 2, SEEK_CUR); // The short value is stored in file with 4 bytes so need to skip 2.
			spriteValues_[type][key] = data;
		}
	}
	fclose(file);
}

bool MapObjectLoader::PlayerIsBehind(sf::IntRect& area, const sf::Vector2i& cellPosition, const std::vector<bool>* transparentPixels, sf::RenderTarget& renderTarget)
{
	// When the player is underneath an object there is no need to check further.
	if (playerPositionComponent_->GetCellPosition().y > cellPosition.y)
	{
		return false;
	}

	// The condition where player area is not even within the object area.
	const sf::IntRect& playerArea = playerAreaComponent_->GetArea();
	if (!playerArea.intersects(area))
	{
		return false;
	}

	// Check along a Line going through player area at the middle from top to bottom at a fixed height.
	// Height is fixed because the height of the player is not static.
	sf::Vector2i point(playerArea.left + (playerArea.width / 2), playerArea.top);
	for (int y = 0; y < 90; ++y, ++point.y)
	{
		if (area.contains(point) && !transparentPixels->at((point.y-area.top)*area.width+(point.x-area.left)))
		{
			return true;
		}
	}

	return false;
}

void MapObjectLoader::RenderObjectSprite(sf::Sprite& sprite, ObjectData& data, SpriteBatch& spriteBatch,
	sf::RenderTarget& renderTarget, sf::BlendMode blendMode, sf::Texture* paletteTexture)
{
	if (data.time.getElapsedTime().asSeconds() > 1.f / 20)
	{
		switch (data.fadeType)
		{
		case FadeType::In:
			if (data.alpha < 255)
			{
				data.alpha += 15;
			}
			break;
		case FadeType::Out:
			if (data.alpha > 0)
			{
				data.alpha -= 15;
			}
			break;
		case FadeType::OutHalf:
			if (data.alpha > 127)
			{
				data.alpha -= 15;
			}
			break;
		}
		data.time.restart();
	}

	if (paletteTexture != nullptr)
	{
		sprite.setColor(sf::Color(255, 255, 255, data.alpha));
		spriteBatch.Draw(sprite, paletteTexture, blendMode, &Game::Instance().paletteShader);
	}
}

void MapObjectLoader::RenderObject(const MapObjectType::Enum type, const unsigned int x, const unsigned int y, SpriteBatch& spriteBatch,
	sf::RenderTarget& renderTarget)
{
	if (x < 0 || x >= width_ || y < 0 || y >= height_)
	{
		return;
	}

	ObjectData* data = spriteValues_[type][y*width_+x].get();
	if (data == nullptr)
	{
		return;
	}

	const unsigned char index = data->value >> 8;
	const unsigned char animationNumber = data->value & 0x00FF;
	if (index >= spriteFilenames_[type].size())
	{
		return;
	}

	sf::BlendMode blendMode = sf::BlendAlpha;
	if (spriteFilenames_[type][index][0] == 'b' && spriteFilenames_[type][index][1] == 'l')
	{
		blendMode = sf::BlendAdd;
	}

	boost::shared_ptr<Framework::AnimationResource>& animation =
		Framework::animationCache.Get("map/objects/" + aniFilenames_[type][index]);

	boost::shared_ptr<Framework::TextureAtlasResource>& textureAtlas =
		Framework::textureAtlasCache.Get("map/objects/" + spriteFilenames_[type][index]);

	bool finished;
	unsigned int regionIndex = type == MapObjectType::Shadow
		? animation->GetKeyFrameByFrameNumber(animationNumber, 0, 0)
		: animation->GetKeyFrameByTime(animationNumber, 0, time_.getElapsedTime().asSeconds(), finished, true);

	sf::Sprite sprite;
	if (!textureAtlas->GetRegionAsSprite(regionIndex, sprite))
	{
		return;
	}
	sprite.setPosition(static_cast<float>(x * tileSize_ + 16), static_cast<float>(y * tileSize_ + 16));

	if (type == MapObjectType::Shadow)
	{
		spriteBatch.Draw(sprite, nullptr, blendMode, &Game::Instance().shadowShader);
	}
	else
	{
		if (playerAreaComponent_ == nullptr || playerPositionComponent_ == nullptr)
		{
			playerAreaComponent_ = dynamic_cast<AreaComponent*>(Game::Instance().myEntity->getComponent<AreaComponent>());
			playerPositionComponent_ = dynamic_cast<PositionComponent*>(Game::Instance().myEntity->getComponent<PositionComponent>());
			return;
		}

		sf::IntRect area(sprite.getGlobalBounds());
		const sf::IntRect& playerArea = playerAreaComponent_->GetArea();
		if (type == MapObjectType::HouseOut && playerArea.intersects(area))
		{
			const bool change = houseIndex_ != zone_.GetHouse(playerPositionComponent_->GetCellPosition());
			houseIndex_ = zone_.GetHouse(playerPositionComponent_->GetCellPosition());

			ObjectData* dataIn = spriteValues_[MapObjectType::HouseIn][y*width_+x].get();
			if (change)
			{
				if (houseIndex_ != -1)
				{
					data->fadeType = FadeType::Out;
					dataIn->fadeType = FadeType::In;
				}
				else
				{
					data->fadeType = FadeType::In;
					dataIn->fadeType = FadeType::Out;
				}
			}
			else
			{
				if (houseIndex_ == -1)
				{
					data->fadeType = PlayerIsBehind(area, Utils::PositionConvert::PixelToCell(sprite.getPosition()),
						textureAtlas->GetRegionTransparentPixels(regionIndex), renderTarget) ? FadeType::OutHalf : FadeType::In;
				}
			}

			RenderObjectSprite(sprite, *data, spriteBatch, renderTarget, blendMode, textureAtlas->GetRegionPaletteTexture(regionIndex));
			boost::shared_ptr<Framework::TextureAtlasResource>& textureAtlasIn = Framework::textureAtlasCache.Get("map/objects/" + spriteFilenames_[MapObjectType::HouseIn][index]);
			if (textureAtlasIn->GetRegionAsSprite(regionIndex, sprite))
			{
				RenderObjectSprite(sprite, *dataIn, spriteBatch, renderTarget, blendMode, textureAtlasIn->GetRegionPaletteTexture(regionIndex));
			}
		}
		else
		{
			data->fadeType = PlayerIsBehind(area, Utils::PositionConvert::PixelToCell(sprite.getPosition()),
				textureAtlas->GetRegionTransparentPixels(regionIndex), renderTarget) ? FadeType::OutHalf : FadeType::In;
			RenderObjectSprite(sprite, *data, spriteBatch, renderTarget, blendMode, textureAtlas->GetRegionPaletteTexture(regionIndex));
		}
	}
}