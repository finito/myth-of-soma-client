
#include "Components/ExpiresComponent.h"

using namespace artemis;

ExpiresComponent::ExpiresComponent(float lifeTime) :
	lifeTime_(lifeTime)
{
}

float ExpiresComponent::GetLifeTime() const
{
	return lifeTime_;
}

void ExpiresComponent::SetLifeTime(float value)
{
	lifeTime_ = value;
}

void ExpiresComponent::ReduceLifeTime(float value)
{
	lifeTime_ -= value;
}

bool ExpiresComponent::IsExpired() const
{
	return lifeTime_ <= 0;
}