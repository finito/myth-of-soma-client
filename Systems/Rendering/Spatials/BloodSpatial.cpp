
#include "Systems/Rendering/Spatials/BloodSpatial.h"

#include "Framework/Assets/ResourceCache.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Components/BloodComponent.h"

#include <boost/format.hpp>

#include "Game.h"

using namespace artemis;
using namespace Framework;

namespace Spatials
{

Blood::Blood(World& world, Entity& entity) :
Spatial(world, entity)
{
}

void Blood::Initialize()
{
	Spatial::Initialize();
	bloodComponent_ = entity.getComponent<BloodComponent>();
	textureAtlas_ = textureAtlasCache.Get(boost::str(boost::format("magicres/blood%1$03i.dat") % bloodComponent_->GetType()));
}

void Blood::Render(SpriteBatch& spriteBatch)
{
	sf::Sprite sprite;
	sprite.setPosition(positionComponent_->GetPosition());

	if (textureAtlas_->GetRegionAsSprite(bloodComponent_->GetKeyFrame(), sprite))
	{
		if (bloodComponent_->GetMaximumKeyFrame() == 0)
		{
			bloodComponent_->SetMaximumKeyFrame(textureAtlas_->GetRegionCount() - 1);
		}
	}

	sf::Texture* paletteTexture = textureAtlas_->GetRegionPaletteTexture(bloodComponent_->GetKeyFrame());
	if (paletteTexture != nullptr)
	{
		spriteBatch.Draw(sprite, paletteTexture, &Game::Instance().paletteShader);
	}
}

} // namespace Spatials