
#include "Systems/Rendering/Spatials/ArrowSpatial.h"

#include "Artemis//ComponentMapper.h"

#include "Framework/Assets/ResourceCache.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Components/AnimationComponent.h"
#include "Components/ArrowComponent.h"

#include <boost/format.hpp>
#include "Game.h"

using namespace artemis;
using namespace Framework;

namespace Spatials
{

Arrow::Arrow(World& world, Entity& entity) :
Spatial(world, entity)
{
}

void Arrow::Initialize()
{
	Spatial::Initialize();
	animationComponent_ = entity.getComponent<AnimationComponent>();
	arrowComponent_ = entity.getComponent<ArrowComponent>();

	animationComponent_->direction = arrowComponent_->GetDirection();
	animationComponent_->animation = Framework::animationCache.Get(
		boost::str(boost::format("magicres/arrow%1$03i.ani") % arrowComponent_->GetType()));

	textureAtlas_ = textureAtlasCache.Get(boost::str(boost::format("magicres/arrow%1$03i.dat") % arrowComponent_->GetType()));
}

void Arrow::Render(SpriteBatch& spriteBatch)
{
	sf::Sprite sprite;
	sprite.setPosition(positionComponent_->GetPosition());
	if (textureAtlas_->GetRegionAsSprite(animationComponent_->regionIndex, sprite))
	{
		sf::Texture* paletteTexture = textureAtlas_->GetRegionPaletteTexture(animationComponent_->regionIndex);
		if (paletteTexture != nullptr)
		{
			spriteBatch.Draw(sprite, paletteTexture, &Game::Instance().paletteShader);
		}
	}
}

} // namespace Spatials