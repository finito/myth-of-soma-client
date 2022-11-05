#ifndef COMPONENTS_DEAD_COMPONENT_H
#define COMPONENTS_DEAD_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"

class DeadComponent : public artemis::Component
{
public:
	explicit DeadComponent(float delayTime);

	float GetDelayTime() const;
	void ReduceDelayTime(float value);
	bool IsExpired() const;

private:
	float delayTime_;
};

#endif // COMPONENTS_DEAD_COMPONENT_H