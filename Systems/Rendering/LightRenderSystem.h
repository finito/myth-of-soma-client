#ifndef LIGHT_RENDER_SYSTEM_H
#define LIGHT_RENDER_SYSTEM_H

#include "Artemis/World.h"
#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

#include "Components/PositionComponent.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <boost/scoped_ptr.hpp>

namespace Framework
{
	class SpriteBatch;
	class TextureResource;
}

class CameraSystem;
class MapLight;

class LightRenderSystem : public artemis::EntitySystem
{
private:
	artemis::ComponentMapper<PositionComponent> positionMapper;

public:
	LightRenderSystem(sf::RenderTarget& renderTarget, MapLight& mapLight);
	~LightRenderSystem();

	virtual void initialize();
	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*>& entities);

private:
	bool checkProcessing();

	sf::RenderTarget& renderTarget_;
	CameraSystem* cameraSystem_;
	MapLight& mapLight_;
	boost::scoped_ptr<Framework::SpriteBatch> spriteBatch_;
	boost::shared_ptr<sf::Texture> texture_;
	boost::shared_ptr<sf::Texture> texture2_;
};

#endif // LIGHT_RENDER_SYSTEM_H