#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "Artemis/World.h"
#include "Artemis/EntitySystem.h"
#include "Artemis/ComponentMapper.h"

#include "Components/SpatialComponent.h"
#include "Components/BloodComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/AreaComponent.h"
#include "Components/PositionComponent.h"
#include "Components/EffectComponent.h"


#include <SFML/Graphics/RenderTarget.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

class CameraSystem;
class BloodSystem;
class SpatialComponent;
class MapObjectLoader;
class Zone;

namespace Framework
{
	class SpriteBatch;
}

namespace Spatials
{
	class Spatial;
}

class RenderSystem : public artemis::EntitySystem
{
private:
	artemis::ComponentMapper<SpatialComponent> spatialMapper;
	artemis::ComponentMapper<BloodComponent> bloodMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
	artemis::ComponentMapper<AreaComponent> areaMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
	artemis::ComponentMapper<EffectComponent> effectMapper;

public:
	RenderSystem(Zone& zone, sf::RenderTarget& renderTarget);
	~RenderSystem();

	virtual void initialize();

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*>& entities);

	sf::IntRect GetSpatialArea(artemis::Entity& entity);

protected:
	void added(artemis::Entity& entity);
	void removed(artemis::Entity& entity);

	void begin();
	void end();

private:
	bool checkProcessing();

	boost::shared_ptr<Spatials::Spatial> GetSpatial(artemis::Entity& e);

	void RenderTiledObjects(unsigned int yLimit, const sf::Vector2<unsigned int>& xRange, unsigned int& yLimitPrevious);
	void RenderCharacterBlood(artemis::Entity& e);
	void RenderPreEffects();
	void RenderPostEffects();
	void RenderArrows();

	sf::RenderTarget& renderTarget_;
	Zone& zone_;
	MapObjectLoader& mapObjectLoader_;
	CameraSystem* cameraSystem_;
	BloodSystem* bloodSystem_;
	std::map<int, boost::shared_ptr<Spatials::Spatial>> characterLayer_;
	std::map<int, boost::shared_ptr<Spatials::Spatial>> itemLayer_;
	std::map<int, boost::shared_ptr<Spatials::Spatial>> preEffectLayer_;
	std::map<int, boost::shared_ptr<Spatials::Spatial>> postEffectLayer_;
	std::map<int, boost::shared_ptr<Spatials::Spatial>> bloodLayer_;
	std::map<int, boost::shared_ptr<Spatials::Spatial>> arrowLayer_;
	std::unique_ptr<Framework::SpriteBatch> spriteBatch_;
	int houseIndex_;
};

#endif // RENDER_SYSTEM_H