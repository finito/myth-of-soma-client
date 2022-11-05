#ifndef SYSTEMS_EFFECT_SYSTEM_H
#define SYSTEMS_EFFECT_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

class EffectComponent;
class AttributeComponent;
class PositionComponent;

class EffectSystem : public artemis::EntityProcessingSystem
{
public:
	EffectSystem();

	virtual void initialize();

	virtual void processEntity(artemis::Entity& e);

private:
	artemis::ComponentMapper<EffectComponent> effectMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
};

#endif // SYSTEMS_EFFECT_SYSTEM_H