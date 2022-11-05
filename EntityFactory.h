#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "Artemis/Entity.h"
#include "Artemis/EntityManager.h"
#include "Artemis/World.h"

#include "Components/AnimationComponent.h"
#include "Components/SpatialComponent.h"
#include "Components/PositionComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/ItemAttributeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/FocusComponent.h"
#include "Components/AreaComponent.h"
#include "Components/MovementComponent.h"
#include "Components/ChatComponent.h"
#include "Components/BloodComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/MagicComponent.h"
#include "Components/EffectComponent.h"
#include "Components/FlyingEffectComponent.h"

#include "DataTables/MagicTable.h"

#include <boost/make_shared.hpp>

class EntityFactory
{
public:
	static artemis::Entity& CreateCharacter(artemis::World& world, sf::Vector2i cellPosition)
	{
		artemis::EntityManager* entityManager = world.getEntityManager();
		artemis::Entity& e = entityManager->create();
		e.addComponent(new AnimationComponent());
		e.addComponent(new PositionComponent(cellPosition));
		e.addComponent(new SpatialComponent("character"));
		e.addComponent(new AttributeComponent());
		e.addComponent(new InventoryComponent());
		e.addComponent(new FocusComponent("character"));
		e.addComponent(new AreaComponent());
		e.addComponent(new MovementComponent());
		e.addComponent(new ChatComponent());
		e.refresh();
		return e;
	}

	static artemis::Entity& CreateItem(artemis::World& world, sf::Vector2i cellPosition)
	{
		artemis::EntityManager* entityManager = world.getEntityManager();
		artemis::Entity& e = entityManager->create();
		e.addComponent(new PositionComponent(cellPosition));
		e.addComponent(new SpatialComponent("item"));
		e.addComponent(new ItemAttributeComponent());
		e.addComponent(new FocusComponent("item"));
		e.addComponent(new AreaComponent());
		e.refresh();
		return e;
	}

	static artemis::Entity& CreateBlood(artemis::World& world, artemis::Entity& parent, int type, bool attack)
	{
		artemis::EntityManager* entityManager = world.getEntityManager();
		artemis::Entity& e = entityManager->create();
		e.addComponent(new BloodComponent(parent, type, attack));
		e.addComponent(new PositionComponent(parent.getComponent<PositionComponent>()->GetPosition()));
		e.addComponent(new SpatialComponent("blood"));
		e.refresh();
		return e;
	}

	static artemis::Entity& CreateArrow(artemis::World& world, artemis::Entity& parent, artemis::Entity& target, int type, bool hit)
	{
		artemis::EntityManager* entityManager = world.getEntityManager();
		artemis::Entity& e = entityManager->create();
		e.addComponent(new ArrowComponent(parent, target, type, hit));
		e.addComponent(new AnimationComponent(true));
		e.addComponent(new PositionComponent(parent.getComponent<PositionComponent>()->GetPosition()));
		e.addComponent(new SpatialComponent("arrow"));
		e.refresh();
		return e;
	}

	static artemis::Entity& CreateMagic(artemis::World& world, artemis::Entity& source, const MagicTable::MagicElement& magicTable)
	{
		artemis::EntityManager* entityManager = world.getEntityManager();
		artemis::Entity& e = entityManager->create();
		e.addComponent(new MagicComponent(source, magicTable));
		e.refresh();
		return e;
	}

	static artemis::Entity& CreateEffect(artemis::World& world, artemis::Entity& parent, EffectType::Enum type, const std::string& filename, const unsigned int id)
	{
		artemis::EntityManager* entityManager = world.getEntityManager();
		artemis::Entity& e = entityManager->create();
		e.addComponent(new EffectComponent(parent, type, filename, id, true));
		e.addComponent(new PositionComponent(parent.getComponent<PositionComponent>()->GetPosition()));
		e.addComponent(new AnimationComponent());
		e.addComponent(new SpatialComponent("effect"));
		e.refresh();
		return e;
	}

	static artemis::Entity& CreateFlyingEffect(artemis::World& world, artemis::Entity& source, artemis::Entity& target, const std::string& filename)
	{
		artemis::EntityManager* entityManager = world.getEntityManager();
		artemis::Entity& e = entityManager->create();
		e.addComponent(new EffectComponent(source, EffectType::Pre, filename, 0, false));
		e.addComponent(new FlyingEffectComponent(source, target, filename));
		e.addComponent(new PositionComponent(source.getComponent<PositionComponent>()->GetPosition()));
		e.addComponent(new AnimationComponent());
		e.addComponent(new SpatialComponent("effect"));
		e.refresh();
		return e;
	}
};

#endif // ENTITY_FACTORY_H