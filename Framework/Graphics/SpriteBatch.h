#ifndef SPRITE_BATCH_H
#define SPRITE_BATCH_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <map>
#include <vector>

namespace sf
{
	class Sprite;
	class Shader;
}

namespace Framework
{

// Simple sprite batching for SFML sprite.
class SpriteBatch
{
public:
	SpriteBatch(sf::RenderTarget& renderTarget);
	~SpriteBatch();

	void Begin(bool sortEnabled = false);
	void Draw(const sf::Sprite& sprite, const sf::Texture* paletteTexture, const sf::Shader* shader = nullptr);
	void Draw(const sf::Sprite& sprite, const sf::Texture* paletteTexture, const sf::BlendMode& blendMode, const sf::Shader* shader = nullptr);
	void End();

private:
	// Draw the sprites to the render target in batches.
	void DrawSprites();

	static const size_t MaxBatchSize = 2048;

	struct SpriteInfo
	{
		const sf::Texture* texture;
		const sf::Texture* paletteTexture;
		sf::Vertex vertices[4];
	};

	std::unique_ptr<SpriteInfo[]> spriteQueue_;
	size_t spriteQueueCount_;
	bool drawing_;
	sf::RenderTarget& renderTarget_;
	sf::BlendMode lastBlendMode_;
	sf::RenderStates states_;
	std::vector<const SpriteInfo*> sortedSprites_;
	sf::Vertex vertices_[8192];
	bool sortEnabled_;
};

} // namespace Framework

#endif // SPRITE_BATCH_H