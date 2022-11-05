
#include "SpriteBatch.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shader.hpp>

namespace Framework
{

SpriteBatch::SpriteBatch(sf::RenderTarget& renderTarget) :
	spriteQueue_(new SpriteInfo[MaxBatchSize]),
	spriteQueueCount_(0),
	drawing_(false),
	renderTarget_(renderTarget)
{
}

SpriteBatch::~SpriteBatch()
{
}

void SpriteBatch::Begin(const bool sortEnabled)
{
	if (drawing_)
	{
		return;
	}
	sortEnabled_ = sortEnabled;
	drawing_ = true;
	lastBlendMode_ = sf::BlendAlpha;
}

void SpriteBatch::End()
{
	if (!drawing_)
	{
		return;
	}

	DrawSprites();

	spriteQueueCount_ = 0;
	drawing_ = false;
	lastBlendMode_ = sf::BlendAlpha;
	states_.shader = nullptr;
}

void SpriteBatch::Draw(const sf::Sprite& sprite, const sf::Texture* paletteTexture, const sf::Shader* shader)
{
	if (states_.shader != shader)
	{
		DrawSprites();
		states_.shader = shader;
	}

	if (lastBlendMode_ != sf::BlendAlpha)
	{
		DrawSprites();
		lastBlendMode_ = sf::BlendAlpha;
	}

	if (spriteQueueCount_ == MaxBatchSize)
	{
        DrawSprites();
    }

	// Pre-transform the vertices
	const sf::IntRect& textureRect = sprite.getTextureRect();

	float width  = static_cast<float>(textureRect.width);
	float height = static_cast<float>(textureRect.height);

	SpriteInfo* spriteInfo = &spriteQueue_[spriteQueueCount_];

	spriteInfo->texture = sprite.getTexture();
	spriteInfo->paletteTexture = paletteTexture;

	const sf::Transform& transform = sprite.getTransform();
	spriteInfo->vertices[0].position = transform * sf::Vector2f(0, 0);
	spriteInfo->vertices[1].position = transform * sf::Vector2f(0, height);
	spriteInfo->vertices[2].position = transform * sf::Vector2f(width, height);
	spriteInfo->vertices[3].position = transform * sf::Vector2f(width, 0);

	spriteInfo->vertices[0].color = sprite.getColor();
	spriteInfo->vertices[1].color = spriteInfo->vertices[0].color;
	spriteInfo->vertices[2].color = spriteInfo->vertices[0].color;
	spriteInfo->vertices[3].color = spriteInfo->vertices[0].color;

	float left   = static_cast<float>(textureRect.left);
	float right  = left + textureRect.width;
	float top    = static_cast<float>(textureRect.top);
	float bottom = top + textureRect.height;

	spriteInfo->vertices[0].texCoords = sf::Vector2f(left, top);
	spriteInfo->vertices[1].texCoords = sf::Vector2f(left, bottom);
	spriteInfo->vertices[2].texCoords = sf::Vector2f(right, bottom);
	spriteInfo->vertices[3].texCoords = sf::Vector2f(right, top);

	spriteQueueCount_++;
}

void SpriteBatch::Draw(const sf::Sprite& sprite, const sf::Texture* paletteTexture, const sf::BlendMode& blendMode, const sf::Shader* shader)
{
	if (states_.shader != shader)
	{
		DrawSprites();
		states_.shader = shader;
	}

	if (lastBlendMode_ != blendMode)
	{
		DrawSprites();
		lastBlendMode_ = blendMode;
	}

	if (spriteQueueCount_ == MaxBatchSize)
	{
		DrawSprites();
	}

	// Pre-transform the vertices
	const sf::IntRect& textureRect = sprite.getTextureRect();

	float width  = static_cast<float>(textureRect.width);
	float height = static_cast<float>(textureRect.height);

	SpriteInfo* spriteInfo = &spriteQueue_[spriteQueueCount_];

	spriteInfo->texture = sprite.getTexture();
	spriteInfo->paletteTexture = paletteTexture;

	const sf::Transform& transform = sprite.getTransform();
	spriteInfo->vertices[0].position = transform * sf::Vector2f(0, 0);
	spriteInfo->vertices[1].position = transform * sf::Vector2f(0, height);
	spriteInfo->vertices[2].position = transform * sf::Vector2f(width, height);
	spriteInfo->vertices[3].position = transform * sf::Vector2f(width, 0);

	spriteInfo->vertices[0].color = sprite.getColor();
	spriteInfo->vertices[1].color = spriteInfo->vertices[0].color;
	spriteInfo->vertices[2].color = spriteInfo->vertices[0].color;
	spriteInfo->vertices[3].color = spriteInfo->vertices[0].color;

	float left   = static_cast<float>(textureRect.left);
	float right  = left + textureRect.width;
	float top    = static_cast<float>(textureRect.top);
	float bottom = top + textureRect.height;

	spriteInfo->vertices[0].texCoords = sf::Vector2f(left, top);
	spriteInfo->vertices[1].texCoords = sf::Vector2f(left, bottom);
	spriteInfo->vertices[2].texCoords = sf::Vector2f(right, bottom);
	spriteInfo->vertices[3].texCoords = sf::Vector2f(right, top);

	spriteQueueCount_++;
}

void SpriteBatch::DrawSprites()
{
	if (spriteQueueCount_ > 0)
	{
		sortedSprites_.resize(spriteQueueCount_);
		for (size_t i = 0; i < spriteQueueCount_; ++i)
		{
			sortedSprites_[i] = &spriteQueue_[i];
		}

		if (sortEnabled_)
		{
			std::sort(sortedSprites_.begin(), sortedSprites_.begin() + spriteQueueCount_, [](const SpriteInfo* left, const SpriteInfo* right) -> bool
			{
				return left->texture < right->texture;
			});
		}

		sf::Texture* batchTexture = nullptr;
		size_t batchStart = 0;
		for (size_t pos = 0; pos < spriteQueueCount_; ++pos)
		{
			sf::Texture* texture =  const_cast<sf::Texture*>(sortedSprites_[pos]->texture);
			if (texture != batchTexture)
			{
				if (pos > batchStart)
				{
					states_.texture = batchTexture;
					states_.blendMode = lastBlendMode_;

					size_t verticesCount = 0;
					size_t batchSize = pos - batchStart;
					for (size_t i = 0; i < batchSize; ++i)
					{
						vertices_[verticesCount++] = sortedSprites_[batchStart+i]->vertices[0];
						vertices_[verticesCount++] = sortedSprites_[batchStart+i]->vertices[1];
						vertices_[verticesCount++] = sortedSprites_[batchStart+i]->vertices[2];
						vertices_[verticesCount++] = sortedSprites_[batchStart+i]->vertices[3];
					}

					if (sortedSprites_[batchStart]->paletteTexture != nullptr)
					{
						const_cast<sf::Shader*>(states_.shader)->setParameter("paletteTexture", *sortedSprites_[batchStart]->paletteTexture);
					}
					renderTarget_.draw(vertices_, verticesCount, sf::Quads, states_);
				}
				batchTexture = texture;
				batchStart = pos;
			}
		}

		size_t verticesCount = 0;
		size_t batchSize = spriteQueueCount_ - batchStart;
		for (size_t i = 0; i < batchSize; ++i)
		{
			vertices_[verticesCount++] = sortedSprites_[batchStart+i]->vertices[0];
			vertices_[verticesCount++] = sortedSprites_[batchStart+i]->vertices[1];
			vertices_[verticesCount++] = sortedSprites_[batchStart+i]->vertices[2];
			vertices_[verticesCount++] = sortedSprites_[batchStart+i]->vertices[3];
		}

		states_.texture = batchTexture;
		states_.blendMode = lastBlendMode_;
		if (sortedSprites_[batchStart]->paletteTexture != nullptr)
		{
			const_cast<sf::Shader*>(states_.shader)->setParameter("paletteTexture", *sortedSprites_[batchStart]->paletteTexture);
		}
		renderTarget_.draw(vertices_, verticesCount, sf::Quads, states_);

		spriteQueueCount_ = 0;
	}
}

} // namespace Framework