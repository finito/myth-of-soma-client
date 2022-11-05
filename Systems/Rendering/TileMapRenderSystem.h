#ifndef TILE_MAP_RENDER_SYSTEM_H
#define TILE_MAP_RENDER_SYSTEM_H

#include "Artemis/World.h"
#include "Artemis/EntityProcessingSystem.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <boost/scoped_ptr.hpp>

#include <list>

#include <SFML/Graphics/Shader.hpp>

namespace Framework
{
	class SpriteBatch;
}

class CameraSystem;
class Zone;
class TileMap;
class MapObjectLoader;

class TileMapRenderSystem : public artemis::EntitySystem
{
public:
	TileMapRenderSystem(sf::RenderTarget& renderTarget, Zone& zone);
	~TileMapRenderSystem();

	virtual void initialize();
	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*>& entities);

private:
	bool checkProcessing();

	static const unsigned int tileSize_ = 32;

	sf::RenderTarget& renderTarget_;
	CameraSystem* cameraSystem_;
	Zone& zone_;
	TileMap& tileMap_;
	MapObjectLoader& mapObjectLoader_;
	boost::scoped_ptr<Framework::SpriteBatch> spriteBatch_;
};

#endif // TILE_MAP_RENDER_SYSTEM_H