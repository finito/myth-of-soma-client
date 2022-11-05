#ifndef SYSTEMS_CHARACTER_STATE_SYSTEM_H
#define SYSTEMS_CHARACTER_STATE_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

class AttributeComponent;
class EquipmentComponent;
class AnimationComponent;

class CharacterStateSystem : public artemis::EntityProcessingSystem
{
public:
	CharacterStateSystem();

	virtual void initialize();

	virtual void processEntity(artemis::Entity& e);

private:
	void SetAnimation();
	unsigned int GetAnimationState() const;

	AttributeComponent* attributeComponent_;
	EquipmentComponent* equipmentComponent_;
	AnimationComponent* animationComponent_;

	artemis::ComponentMapper<AttributeComponent> attributeMapper;
	artemis::ComponentMapper<EquipmentComponent> equipmentMapper;
	artemis::ComponentMapper<AnimationComponent> animationMapper;
};

#endif // SYSTEMS_CHARACTER_STATE_SYSTEM_H