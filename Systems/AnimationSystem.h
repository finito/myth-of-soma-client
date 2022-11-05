#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "Artemis/ComponentMapper.h"
#include "Artemis/EntityProcessingSystem.h"

class AnimationComponent;

class AnimationSystem : public artemis::EntityProcessingSystem
{
public:
	AnimationSystem();

	virtual void initialize();

	virtual void processEntity(artemis::Entity& e);

private:
	artemis::ComponentMapper<AnimationComponent> animationMapper;
};

#endif // ANIMATION_SYSTEM_H