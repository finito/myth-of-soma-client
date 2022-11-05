#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <sstream>

namespace sf
{
	class Sprite;
}

namespace Framework
{

class TextureRegion;
class TextureResource;

class TextureAtlas
{
	typedef std::vector<boost::shared_ptr<TextureRegion>> regionVector;

public:
	bool loadFromFile(const std::string& filename);
	bool loadFromMemory(const void* data, std::size_t size);

	regionVector::size_type GetRegionCount() const;

	TextureRegion* GetRegion(size_t index) const;

	bool GetRegionAsSprite(size_t index, sf::Sprite& sprite) const;

	sf::Texture* GetRegionPaletteTexture(size_t index) const;

	const std::vector<bool>* GetRegionTransparentPixels(size_t index) const;

private:
	bool loadFromStream(std::istream& stream);

	regionVector regions_;
};

} // namespace Framework

#endif // TEXTURE_ATLAS_H