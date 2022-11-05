#ifndef COMPONENTS_EXPIRES_COMPONENT_H
#define COMPONENTS_EXPIRES_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"

class ExpiresComponent : public artemis::Component
{
public:
	explicit ExpiresComponent(float lifeTime);

	float GetLifeTime() const;
	void SetLifeTime(float value);
	void ReduceLifeTime(float value);
	bool IsExpired() const;

private:
	float lifeTime_;
};

#endif // COMPONENTS_EXPIRES_COMPONENT_H