
#include "Systems/Rendering/Spatials/ItemSpatial.h"

#include "Framework/Assets/ResourceCache.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Components/ItemAttributeComponent.h"
#include "Components/AreaComponent.h"

#include <boost/format.hpp>

using namespace artemis;
using namespace Framework;

namespace Spatials
{

Item::Item(World& world, Entity& entity) :
	Spatial(world, entity)
{
}

void Item::Initialize()
{
	Spatial::Initialize();
	ItemAttributeComponent* itemAttributeComponent = entity.getComponent<ItemAttributeComponent>();
	AreaComponent* areaComponent = entity.getComponent<AreaComponent>();

	std::string imageset = "itemicons";
	unsigned short pictureId = itemAttributeComponent->GetPictureId();
	if (pictureId > 5000)
	{
		pictureId -= 5000;
		imageset = "ditemicons";
	}
	const unsigned int iconGroup = ((pictureId - 1) / 81) + 1;
	const unsigned int iconIndex = ((pictureId - 1) % 81);
	const std::string filename = boost::str(boost::format("somagui/imagesets/%1%%2%.png") % imageset % iconGroup);

	boost::shared_ptr<sf::Texture> texture = textureCache.Get(filename, sf::TextureFormat::RGBA8);
	const sf::IntRect iconArea((iconIndex % 9) * 52, (iconIndex / 9) * 52, 52, 52);
	
	sprite_.reset(new sf::Sprite(*texture.get()));
	sprite_->setTextureRect(iconArea);
	sprite_->setScale(32.f / iconArea.width, 32.f / iconArea.height);
	sprite_->setPosition(positionComponent_->GetPosition().x - 16, positionComponent_->GetPosition().y - 16);

	areaComponent->SetArea(sf::IntRect(
		static_cast<int>(sprite_->getPosition().x),
		static_cast<int>(sprite_->getPosition().y),
		48,
		24));
}

void Item::Render(SpriteBatch& spriteBatch)
{
	spriteBatch.Draw(*sprite_, nullptr);
}

} // namespace Spatials