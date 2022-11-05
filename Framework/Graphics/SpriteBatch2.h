#ifndef SPRITE_BATCH2_H
#define SPRITE_BATCH2_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <map>
#include <vector>

namespace sf
{
	class Sprite;
}

namespace Framework
{

// Simple sprite batching for SFML sprite.
class SpriteBatch2
{
public:
	SpriteBatch2(sf::RenderTarget& renderTarget);

	void Begin();
	void Draw(const sf::Sprite& sprite, const sf::Texture* paletteTexture, const sf::Shader* shader = nullptr);
	void End();

private:
	static const int spriteVertexCount = 4;

	// Draw the sprites to the render target in batches.
	void DrawSprites();

	struct SpriteData
	{
		std::vector<sf::Vertex> vertices;
		const sf::Texture* paletteTexture;
	};

	// Vertices for each texture.
	std::map<const sf::Texture*, SpriteData> verticesMap_;

	bool drawing_;
	sf::RenderTarget& renderTarget_;
	sf::RenderStates states_;
};

} // namespace Engine

#endif SPRITE_BATCH2_H