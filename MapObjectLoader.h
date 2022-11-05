#ifndef MAP_OBJECT_LOADER_H
#define MAP_OBJECT_LOADER_H

#include <string>
#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Clock.hpp>
#include <boost/shared_ptr.hpp>

namespace MapObjectType
{
	enum Enum
	{
		Normal,
		Shadow,
		HouseOut,
		HouseIn,
		First = Normal,
		Last  = HouseIn + 1,
	};
}

namespace FadeType
{
	enum Enum
	{
		In,
		Out,
		OutHalf,
	};
}

namespace Framework
{
	class TextureAtlasResource;
	class AnimationResource;
	class SpriteBatch;
}

class AreaComponent;
class PositionComponent;

class Zone;

class MapObjectLoader
{
private:
	struct ObjectData
	{
		short value;
		unsigned char alpha;
		FadeType::Enum fadeType;
		sf::Clock time;
	};

public:
	MapObjectLoader(Zone& zone);

	void Load(const std::string& filename);

	void RenderObject(MapObjectType::Enum type, unsigned int x, unsigned int y, Framework::SpriteBatch& spriteBatch,
		sf::RenderTarget& renderTarget);

private:
	bool PlayerIsBehind(sf::IntRect& area, const sf::Vector2i& cellPosition, const std::vector<bool>* transparentPixels, sf::RenderTarget& renderTarget);

	void RenderObjectSprite(sf::Sprite& sprite, ObjectData& data, Framework::SpriteBatch& spriteBatch,
		sf::RenderTarget& renderTarget, sf::BlendMode blendMode, sf::Texture* paletteTexture);

private:
	static const int maxSprite = 64;
	static const int spriteFilenameLength = 64;
	static const int tileSize_ = 32;

	unsigned int width_;
	unsigned int height_;

	Zone& zone_;

	std::vector<std::string> spriteFilenames_[MapObjectType::Last];
	std::vector<std::string> aniFilenames_[MapObjectType::Last];

	std::vector<boost::shared_ptr<ObjectData>> spriteValues_[MapObjectType::Last];

	sf::Clock time_;

	AreaComponent* playerAreaComponent_;
	PositionComponent* playerPositionComponent_;

	int houseIndex_;
};

#endif // MAP_OBJECT_LOADER_H