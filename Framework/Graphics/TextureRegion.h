#ifndef TEXTURE_REGION_H
#define TEXTURE_REGION_H

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace Framework
{

class TextureResource;

class TextureRegion
{
public:
	TextureRegion(const boost::shared_ptr<sf::Texture>& texture, const boost::shared_ptr<sf::Texture>& paletteTexture,
		std::vector<bool> transparentPixels, const sf::IntRect& rect, const sf::Vector2f& offset);

	sf::Texture* GetTexture() const;
	sf::Texture* GetPaletteTexture() const;
	const sf::IntRect& GetTextureRect() const;
	const sf::Vector2f& GetOffset() const;
	const std::vector<bool>& GetTransparentPixels() const;

private:
	std::vector<bool> transparentPixels_;
	boost::shared_ptr<sf::Texture> texture_;
	boost::shared_ptr<sf::Texture> paletteTexture_;
	sf::IntRect	textureRect_;
	sf::Vector2f offset_;
};

} // namespace Framework

#endif // TEXTURE_REGEION_H