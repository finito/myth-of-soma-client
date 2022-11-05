#ifndef SYSTEMS_DEATH_SYSTEM_H
#define SYSTEMS_DEATH_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

class DeadComponent;
class AttributeComponent;

class DeathSystem : public artemis::EntityProcessingSystem
{
public:
	DeathSystem();

	virtual void DeathSystem::initialize();

	virtual void processEntity(artemis::Entity& e);

private:
	static const float EntityExpireDelay;

	artemis::ComponentMapper<DeadComponent> deadMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
};

#endif // SYSTEMS_DEATH_SYSTEM_H