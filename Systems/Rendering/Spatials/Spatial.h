#ifndef SPATIALS_SPATIAL_H
#define SPATIALS_SPATIAL_H

#include "Artemis/World.h"
#include "Artemis/Entity.h"

#include "Framework/Graphics/SpriteBatch.h"
#include <SFML/System/Vector2.hpp>

#include "Components/PositionComponent.h"

namespace Spatials
{

class Spatial
{
public:
	Spatial(artemis::World& world, artemis::Entity& entity) :world(world), entity(entity) {}
	virtual ~Spatial() {}

	virtual void Initialize()
	{
		positionComponent_ = entity.getComponent<PositionComponent>();
	}

	virtual void Render(Framework::SpriteBatch& spriteBatch) = 0;

	sf::Vector2f GetPosition() const
	{
		return positionComponent_->GetPosition();
	}

	sf::Vector2i GetCellPosition() const
	{
		return positionComponent_->GetCellPosition();
	}

	artemis::Entity& GetEntity() const
	{
		return entity;
	}

protected:
	artemis::World& world;
	artemis::Entity& entity;
	PositionComponent* positionComponent_;
};

} // namespace Spatials

#endif // SPATIALS_SPATIAL_H