
#include "Systems/Rendering/LightRenderSystem.h"
#include "Systems/CameraSystem.h"

#include "Artemis/SystemManager.h"

#include "Framework/Assets/ResourceCache.h"
#include "Framework/Graphics/SpriteBatch.h"
#include "Framework/Assets/TextureResource.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Maplight.h"

using namespace artemis;
using namespace Framework;

LightRenderSystem::LightRenderSystem(sf::RenderTarget& renderTarget, MapLight& mapLight) :
renderTarget_(renderTarget),
mapLight_(mapLight),
spriteBatch_(new SpriteBatch(renderTarget))
{

}

LightRenderSystem::~LightRenderSystem()
{
}

void LightRenderSystem::initialize()
{
	cameraSystem_ = dynamic_cast<CameraSystem*>(world->getSystemManager()->getSystem<CameraSystem>());
	texture_ = textureCache.Get("light.png", sf::TextureFormat::Luminance8);
	texture2_ = textureCache.Get("light2.png", sf::TextureFormat::Luminance8);

	positionMapper.init(*world);
}

void LightRenderSystem::processEntities(artemis::ImmutableBag<artemis::Entity*>& entities)
{
	// TODO: Cache the sprites. The lights are static so can add them to a list and only refresh the cache when map changes. Most of the work done here is creating the sprite.
	renderTarget_.setView(cameraSystem_->GetCamera());

	const float left   = cameraSystem_->GetX() - (cameraSystem_->GetWidth() / 2);
	const float top    = cameraSystem_->GetY() - (cameraSystem_->GetHeight() / 2);
	const float width  = cameraSystem_->GetWidth() * 2;
	const float height = cameraSystem_->GetHeight() * 2;

	const sf::FloatRect area(left, top, width, height);

	sf::Sprite sprite;

	spriteBatch_->Begin();
	for (auto it = mapLight_.GetLights().cbegin(); it != mapLight_.GetLights().cend(); ++it)
	{
		const Framework::Light& light = (*it);
		sprite.setTexture(*texture_.get(), true);
		sprite.setScale(light.scale);
		sprite.setColor(light.color);
		const sf::FloatRect spriteBounds = sprite.getGlobalBounds();
		sprite.setPosition(static_cast<float>(light.position.x) - (spriteBounds.width / 2),
			static_cast<float>(light.position.y) - (spriteBounds.height / 2));

		const sf::FloatRect lightArea(
			light.position.x - (spriteBounds.width / 2),
			light.position.y - (spriteBounds.height / 2),
			spriteBounds.width,
			spriteBounds.height);
		if (area.intersects(lightArea))
		{
			spriteBatch_->Draw(sprite, nullptr, sf::BlendAdd);
		}
	}

	if (world->getTagManager()->isSubscribed("PLAYER"))
	{
		Entity& player = world->getTagManager()->getEntity("PLAYER");
		PositionComponent* positionComponent = positionMapper.get(player);
		if (positionComponent != nullptr)
		{
			sf::Sprite sprite(*texture2_.get());
			sprite.setScale(sf::Vector2f(2.f, 1.5f));
			sprite.setColor(sf::Color::White);
			const sf::FloatRect spriteBounds = sprite.getGlobalBounds();
			sprite.setPosition(positionComponent->GetPosition().x - (spriteBounds.width / 2),
				positionComponent->GetPosition().y - (spriteBounds.height / 2));
			spriteBatch_->Draw(sprite, nullptr, sf::BlendAdd);
		}
	}

	spriteBatch_->End();

	renderTarget_.setView(renderTarget_.getDefaultView());
}

bool LightRenderSystem::checkProcessing()
{
	return true;
}