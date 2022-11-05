#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include "Framework/Assets/AnimationResource.h"
#include <boost/shared_ptr.hpp>

class AnimationComponent : public artemis::Component
{
public:
	AnimationComponent() : regionIndex(0), time(0), animationState(0), loop(false), direction(0), finished(false) {}
	AnimationComponent(bool loop) : regionIndex(0), time(0), animationState(0), loop(loop), direction(0), finished(false) {}

	boost::shared_ptr<Framework::AnimationResource> animation;
	float time;
	unsigned int regionIndex;
	unsigned int animationState;
	bool loop;
	unsigned char direction;
	bool finished;
};

#endif // ANIMATION_COMPONENT_H