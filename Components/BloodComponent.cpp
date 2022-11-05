
#include "Components/BloodComponent.h"
#include "Framework/Utils/RandomNumber.h"

using namespace artemis;

BloodComponent::BloodComponent(Entity& parent, int type, bool attack) :
	parent_(parent),
	time_(0),
	stain_(false),
	attack_(attack),
	keyFrame_(0),
	maximumKeyFrame_(0)
{
	SetType(type);
}

int BloodComponent::GetType() const
{
	return type_;
}

void BloodComponent::SetType(int value)
{
	if (attack_)
	{
		type_ = value * 10 + Utils::Random::Range(0, 4) + 4;
	}
	else
	{
		type_ = value * 10 + Utils::Random::Range(0, 2) + 1;
	}
}

bool BloodComponent::IsAttack() const
{
	return attack_;
}

void BloodComponent::SetAttack(bool value)
{
	attack_ = value;
}

bool BloodComponent::IsStain() const
{
	return stain_;
}

void BloodComponent::SetStain(bool value)
{
	stain_ = value;
}

float BloodComponent::GetTime() const
{
	return time_;
}

void BloodComponent::SetTime(float value)
{
	time_ = value;
}

unsigned int BloodComponent::GetKeyFrame() const
{
	return keyFrame_;
}

void BloodComponent::SetKeyFrame(unsigned int value)
{
	keyFrame_ = value;
}

unsigned int BloodComponent::GetMaximumKeyFrame() const
{
	return maximumKeyFrame_;
}

void BloodComponent::SetMaximumKeyFrame(unsigned int value)
{
	maximumKeyFrame_ = value;
}

Entity& BloodComponent::GetParent() const
{
	return parent_;
}