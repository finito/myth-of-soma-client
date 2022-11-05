
#include "TextureRegion.h"
#include "Framework/Assets/TextureResource.h"

namespace Framework
{

TextureRegion::TextureRegion(const boost::shared_ptr<sf::Texture>& texture, const boost::shared_ptr<sf::Texture>& paletteTexture,
	std::vector<bool> transparentPixels, const sf::IntRect& textureRect, const sf::Vector2f& offset) :
	texture_(texture),
	paletteTexture_(paletteTexture),
	transparentPixels_(transparentPixels),
	textureRect_(textureRect),
	offset_(offset)
{
}

sf::Texture* TextureRegion::GetTexture() const
{
	return texture_.get();
}

sf::Texture* TextureRegion::GetPaletteTexture() const
{
	return paletteTexture_.get();
}

const sf::IntRect& TextureRegion::GetTextureRect() const
{
	return textureRect_;
}

const sf::Vector2f& TextureRegion::GetOffset() const
{
	return offset_;
}

const std::vector<bool>& TextureRegion::GetTransparentPixels() const
{
	return transparentPixels_;
}

} // namespace Framework