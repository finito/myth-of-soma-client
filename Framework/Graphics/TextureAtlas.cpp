
#include "TextureAtlas.h"
#include "TextureRegion.h"
#include "Framework/Assets/ResourceCache.h"
#include <fstream>
#include <iostream>
#include <SFML/Graphics/Sprite.hpp>

namespace Framework
{

bool TextureAtlas::loadFromFile(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
	{
		std::cerr << "TextureAtlas file could not be read filename=" << filename << "\n";
		return false;
	}
	return loadFromStream(file);
}

bool TextureAtlas::loadFromMemory(const void* data, const std::size_t size)
{
	std::istringstream sstream(reinterpret_cast<const char*>(data));
	return loadFromStream(sstream);
}

bool TextureAtlas::loadFromStream(std::istream& stream)
{
	unsigned int textureCount;
	stream >> textureCount;

	for (unsigned int i = 0; i < textureCount; ++i)
	{
		std::string textureFilename;
		std::getline(stream, textureFilename); // FIXME: Should not need two of these,
		//		 it is because not reading lines
		//		 so extra is needed to skip a line.
		std::getline(stream, textureFilename);

		unsigned int regionCount;
		stream >> regionCount;
		if (regionCount == 0)
		{
			std::cerr << "TextureAtlas resource does not contain any regions" << std::endl;
			return false;
		}

		boost::shared_ptr<sf::Texture> paletteTexture = textureCache.Get(textureFilename + ".palette", sf::TextureFormat::RGB8);
		boost::shared_ptr<sf::Texture> texture = textureCache.Get(textureFilename + ".index", sf::TextureFormat::Luminance8);		
		sf::Image image = texture->copyToImage();
		sf::Image paletteImage = paletteTexture->copyToImage();

		regions_.reserve(regionCount);
		for (unsigned int i = 0; i < regionCount; ++i)
		{
			sf::IntRect rect;
			stream >> rect.left >> rect.top >> rect.width >> rect.height;

			rect.width = std::min(image.getSize().x, static_cast<unsigned int>(rect.width));
			rect.height = std::min(image.getSize().y, static_cast<unsigned int>(rect.height));

			sf::Vector2f offset;
			stream >> offset.x >> offset.y;

			std::vector<bool> transparentPixels;
			transparentPixels.resize(rect.width * rect.height);
			//for (int x = rect.left, x2 = 0; x < rect.left + rect.width; ++x, ++x2)
			//{
			//	for (int y = rect.top, y2 = 0; y < rect.top + rect.height; ++y, ++y2)
			//	{
			//		sf::Color color = paletteImage.getPixel(image.getPixel(x, y).r, 0);
			//		if (color.r == 255 && color.g == 0 && color.b == 255)
			//		{
			//			transparentPixels[y2*rect.width+x2] = true;
			//		}
			//		else
			//		{
			//			transparentPixels[y2*rect.width+x2] = false;
			//		}
			//	}
			//}

			regions_.push_back(boost::make_shared<TextureRegion>(texture, paletteTexture, transparentPixels, rect, offset));
		}
	}

	return true;
}

TextureAtlas::regionVector::size_type TextureAtlas::GetRegionCount() const
{
	return regions_.size();
}

TextureRegion* TextureAtlas::GetRegion(const size_t index) const
{
	if (index >= regions_.size())
	{
		return nullptr;
	}
	return regions_[index].get();
}

bool TextureAtlas::GetRegionAsSprite(size_t index, sf::Sprite& sprite) const
{
	if (index >= regions_.size())
	{
		if (regions_.size() > 0)
		{
			index = 0;
		}
		else
		{
			return false;
		}
	}
	sprite.setTexture(*regions_[index]->GetTexture());
	sprite.setTextureRect(regions_[index]->GetTextureRect());
	sprite.setOrigin(-regions_[index]->GetOffset());
	return true;
}

sf::Texture* TextureAtlas::GetRegionPaletteTexture(size_t index) const
{
	if (index >= regions_.size())
	{
		if (regions_.size() > 0)
		{
			index = 0;
		}
		else
		{
			return nullptr;
		}
	}

	return regions_[index]->GetPaletteTexture();
}

const std::vector<bool>* TextureAtlas::GetRegionTransparentPixels(const size_t index) const
{
	if (index >= regions_.size())
	{
		return nullptr;
	}
	return &regions_[index]->GetTransparentPixels();
}

} // namespace Framework