#ifndef DEBUG_STATE_H
#define DEBUG_STATE_H

#include "Framework/GameEngine.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>

class TileMap;
class MapLight;

namespace artemis
{
	class World;
}

class RenderSystem;
class CameraSystem;
class TileMapRenderSystem;
class LightRenderSystem;
class AnimationSystem;

class DebugState : public Framework::IGameState
{
public:
	DebugState(Framework::GameEngine& engine);
	~DebugState();

protected:
	virtual void DoUpdate(float deltaTime);
	virtual void DoRender();

private:
	Framework::GameEngine& engine_;
	TileMap* tileMap_;
	MapLight* mapLight_;
	sf::RenderTexture diffuseTexture_;
	sf::RenderTexture resultTexture_;
	sf::Shader lightShader_;
	artemis::World* world_;
	boost::shared_ptr<RenderSystem> renderSystem_;
	boost::shared_ptr<CameraSystem> cameraSystem_;
	boost::shared_ptr<TileMapRenderSystem> tileMapRenderSystem_;
	boost::shared_ptr<LightRenderSystem> lightRenderSystem_;
	boost::shared_ptr<AnimationSystem> animationSystem_;
};

#endif // DEBUG_STATE_H