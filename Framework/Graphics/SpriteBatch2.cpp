
#include "Framework/Graphics/SpriteBatch2.h"
#include <SFML/Graphics/Sprite.hpp>

namespace Framework
{

SpriteBatch2::SpriteBatch2(sf::RenderTarget& renderTarget) :
	drawing_(false),
	renderTarget_(renderTarget)
{

}

void SpriteBatch2::Begin()
{
	if (drawing_)
	{
		return;
	}
	drawing_ = true;
}

void SpriteBatch2::End()
{
	if (!drawing_)
	{
		return;
	}

	DrawSprites();

	drawing_ = false;
	states_.shader = nullptr;
}

void SpriteBatch2::Draw(const sf::Sprite& sprite, const sf::Texture* paletteTexture, const sf::Shader* shader)
{
	if (states_.shader != shader)
	{
		DrawSprites();
		states_.shader = shader;
	}

	// Pre-transform the vertices
	const sf::IntRect& textureRect = sprite.getTextureRect();

	float width  = static_cast<float>(textureRect.width);
	float height = static_cast<float>(textureRect.height);

	sf::Vertex vertices[spriteVertexCount];

	const sf::Transform& transform = sprite.getTransform();
	vertices[0].position = transform * sf::Vector2f(0, 0);
	vertices[1].position = transform * sf::Vector2f(0, height);
	vertices[2].position = transform * sf::Vector2f(width, height);
	vertices[3].position = transform * sf::Vector2f(width, 0);

	vertices[0].color = sprite.getColor();
	vertices[1].color = vertices[0].color;
	vertices[2].color = vertices[0].color;
	vertices[3].color = vertices[0].color;

	float left   = static_cast<float>(textureRect.left);
	float right  = left + textureRect.width;
	float top    = static_cast<float>(textureRect.top);
	float bottom = top + textureRect.height;

	vertices[0].texCoords = sf::Vector2f(left, top);
	vertices[1].texCoords = sf::Vector2f(left, bottom);
	vertices[2].texCoords = sf::Vector2f(right, bottom);
	vertices[3].texCoords = sf::Vector2f(right, top);

	for (int i = 0; i < spriteVertexCount; ++i)
	{
		verticesMap_[sprite.getTexture()].vertices.push_back(vertices[i]);
	}
	verticesMap_[sprite.getTexture()].paletteTexture = paletteTexture;
}

void SpriteBatch2::DrawSprites()
{
	// TODO: Split large batches into smaller batches. (A maximum count per batch)
	for (auto it = verticesMap_.begin(); it != verticesMap_.end(); ++it)
	{
		states_.texture = (*it).first;
		const_cast<sf::Shader*>(states_.shader)->setParameter("paletteTexture", *(*it).second.paletteTexture);
		renderTarget_.draw(&(*it).second.vertices[0], (*it).second.vertices.size(), sf::Quads, states_);
	}
	verticesMap_.clear();
}

} // namespace Engine