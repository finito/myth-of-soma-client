
#include "DebugState.h"
#include "Game.h"

#include "TileMap.h"
#include "MapLight.h"

#include "Artemis/World.h"

#include "EntityFactory.h"

#include "Systems/CameraSystem.h"
#include "Systems/AnimationSystem.h"

#include "Systems/Rendering/RenderSystem.h"
#include "Systems/Rendering/TileMapRenderSystem.h"
#include "Systems/Rendering/LightRenderSystem.h"

#include "Framework/Assets/AsyncFileLoad.h"
#include "Framework/Assets/ResourceCache.h"
#include "Framework/Graphics/TextureRegion.h"

using namespace artemis;

DebugState::DebugState(Framework::GameEngine& engine) :
engine_(engine),
tileMap_(new TileMap),
mapLight_(new MapLight),
world_(new World)
{
	//Framework::textureCache.SetBasePath("/");
	//Framework::textureAtlasCache.SetBasePath("/");
	//Framework::animationCache.SetBasePath("/");

	tileMap_->Load("map/mset01");

	mapLight_->Load("mset01.lsd");

	diffuseTexture_.create(engine_.renderWindow.getSize().x, engine_.renderWindow.getSize().y);
	//lightTexture_.Create(engine_.renderWindow.GetWidth(), engine_.renderWindow.GetHeight());

	//lightShader_.LoadFromFile("shaders/light.frag", sf::Shader::Fragment);
	//lightShader_.SetParameter("diffuseTexture", sf::Shader::CurrentTexture);
	//lightShader_.SetParameter("lightMapTexture", lightTexture_.GetTexture());

	//SystemManager* systemManager = world->getSystemManager();
	//EntityManager* entityManager = world->getEntityManager();
	//TagManager* tagManager = world->getTagManger();

	//std::list<ComponentId> renderSystemComponentIds;
	//renderSystemComponentIds.push_back(4);
	//renderSystemComponentIds.push_back(8);

	//renderSystem_ = boost::make_shared<RenderSystem>(diffuseTexture_, renderSystemComponentIds, *world_);
	//systemManager.addSystem(renderSystem_);

	//cameraSystem_ = boost::make_shared<CameraSystem>(engine_.renderWindow, std::list<ComponentId>(), *world_);
	//systemManager.addSystem(cameraSystem_);

	//tileMapRenderSystem_ = boost::make_shared<TileMapRenderSystem>(diffuseTexture_, *tileMap_, std::list<ComponentId>(), *world_);
	//systemManager.addSystem(tileMapRenderSystem_);

	//lightRenderSystem_= boost::make_shared<RenderSystem>(lightTexture_, *mapLight_, std::list<ComponentId>(), *world);
	//systemManager.addSystem(lightRenderSystem_);

	//std::list<ComponentId> animationSystemComponentIds;
	//animationSystemComponentIds.push_back(2);
	//animationSystem_ = boost::make_shared<AnimationSystem>(animationSystemComponentIds, *world_);
	//systemManager.addSystem(animationSystem_);

	//systemManager.initializeAll();

	//Entity& player = EntityFactory::CreateCharacter(*world_, sf::Vector2i(0, 0));
	//tagManager->subscribe("PLAYER", player);
}

DebugState::~DebugState()
{
}

void DebugState::DoUpdate(const float deltaTime)
{
    sf::Event e;
    while (engine_.renderWindow.pollEvent(e))
    {
		if (!engine_.gui.HandleEvent(engine_.renderWindow, e))
		{
			if (e.type == sf::Event::Closed)
			{
				engine_.Quit();
			}
		}
    }

	//Framework::PollAsyncLoads();

	//Framework::textureCache.CheckLoad();
	//Framework::textureAtlasCache.CheckLoad();
	//Framework::animationCache.CheckLoad();

	//world_->loopStart();
	//world_->set_delta(deltaTime * 1000);

	//inputSystem_->process();
	//cameraSystem_->process();
	//animationSystem_->process();
}

void DebugState::DoRender()
{
	//diffuseTexture_.clear();
	//tileMapRenderSystem_->process();
	//renderSystem_->process();
	//diffuseTexture_.display();

	////lightTexture.Clear();
	////lightRenderSystem->process();
	////lightTexture.Display();

	//sf::RenderStates states;
	////states.Shader = &lightShader;
	//engine_.renderWindow.draw(sf::Sprite(diffuseTexture_.getTexture()), states);
}