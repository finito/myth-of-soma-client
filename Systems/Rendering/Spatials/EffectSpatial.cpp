
#include "Systems/Rendering/Spatials/EffectSpatial.h"

#include "Framework/Assets/ResourceCache.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Components/AnimationComponent.h"
#include "Components/EffectComponent.h"

#include <boost/format.hpp>

#include "Game.h"

using namespace artemis;
using namespace Framework;

namespace Spatials
{

Effect::Effect(World& world, Entity& entity) :
	Spatial(world, entity)
{
}

void Effect::Initialize()
{
	Spatial::Initialize();
	animationComponent_ = entity.getComponent<AnimationComponent>();
	effectComponent_ = entity.getComponent<EffectComponent>();
	textureAtlas_ = textureAtlasCache.Get(effectComponent_->GetFilename());
}

void Effect::Render(SpriteBatch& spriteBatch)
{
	sf::Sprite sprite;
	sprite.setPosition(positionComponent_->GetPosition());
	if (textureAtlas_->GetRegionAsSprite(animationComponent_->regionIndex, sprite))
	{
		sf::Texture* paletteTexture = textureAtlas_->GetRegionPaletteTexture(animationComponent_->regionIndex);
		if (paletteTexture != nullptr)
		{
			spriteBatch.Draw(sprite, paletteTexture, sf::BlendAdd, &Game::Instance().paletteShader);
		}
	}
}

} // namespace Spatials