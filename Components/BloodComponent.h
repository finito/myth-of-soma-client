#ifndef COMPONENTS_BLOOD_COMPONENT_H
#define COMPONENTS_BLOOD_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <vector>

class BloodComponent : public artemis::Component
{
public:
	BloodComponent(artemis::Entity& parent, int type, bool attack);

	int GetType() const;
	void SetType(int value);

	bool IsAttack() const;
	void SetAttack(bool value);

	bool IsStain() const;
	void SetStain(bool value);

	float GetTime() const;
	void SetTime(float value);	

	unsigned int GetKeyFrame() const;
	void SetKeyFrame(unsigned int value);

	unsigned int GetMaximumKeyFrame() const;
	void SetMaximumKeyFrame(unsigned int value);

	artemis::Entity& GetParent() const;

private:
	artemis::Entity& parent_;
	int type_;
	float time_;
	bool stain_;
	bool attack_;
	unsigned int keyFrame_;
	unsigned int maximumKeyFrame_;
};

#endif // COMPONENTS_BLOOD_COMPONENT_H