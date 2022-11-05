#ifndef SYSTEMS_FLYING_EFFECT_SYSTEM_H
#define SYSTEMS_FLYING_EFFECT_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

#include "Components/EffectComponent.h" // Have to include for EffectType::Enum

class FlyingEffectComponent;
class PositionComponent;
class AttributeComponent;
class ExpiresComponent;
class AnimationComponent;

class FlyingEffectSystem : public artemis::EntityProcessingSystem
{
public:
	FlyingEffectSystem();

	virtual void initialize();

	virtual void processEntity(artemis::Entity& e);

protected:
	void added(artemis::Entity& e);

private:
	bool SetRoute(artemis::Entity& e);
	void CreateHitEffectEntity(unsigned int effectId, EffectType::Enum type);

private:
	FlyingEffectComponent* flyingEffectComponent_;

	artemis::ComponentMapper<FlyingEffectComponent> flyingEffectMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
	artemis::ComponentMapper<ExpiresComponent> expiresMapper;
	artemis::ComponentMapper<AnimationComponent> animationMapper;
	artemis::ComponentMapper<EffectComponent> effectMapper;
};

#endif // SYSTEMS_FLYING_EFFECT_SYSTEM_H