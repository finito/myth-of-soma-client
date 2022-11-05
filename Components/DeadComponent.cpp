
#include "Components/DeadComponent.h"

using namespace artemis;

DeadComponent::DeadComponent(float delayTime) :
	delayTime_(delayTime)
{
}

float DeadComponent::GetDelayTime() const
{
	return delayTime_;
}

void DeadComponent::ReduceDelayTime(float value)
{
	delayTime_ -= value;
}

bool DeadComponent::IsExpired() const
{
	return delayTime_ <= 0;
}