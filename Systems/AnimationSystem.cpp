
#include "Systems/AnimationSystem.h"

#include "Components/AnimationComponent.h"

using namespace artemis;

AnimationSystem::AnimationSystem()
{
	addComponentType<AnimationComponent>();
}

void AnimationSystem::initialize()
{
	animationMapper.init(*world);
}

void AnimationSystem::processEntity(Entity& e)
{
	if (animationMapper.get(e)->animation)
	{
		animationMapper.get(e)->time += world->getDelta();
		animationMapper.get(e)->regionIndex = animationMapper.get(e)->animation->GetKeyFrameByTime(
			0, animationMapper.get(e)->direction, animationMapper.get(e)->time, animationMapper.get(e)->finished, animationMapper.get(e)->loop);
	}
}