
#include "Components/FlyingEffectComponent.h"

using namespace artemis;

FlyingEffectComponent::FlyingEffectComponent(Entity& source, Entity& target, const std::string& filename) :
	source_(source),
	target_(target),
	filename_(filename),
	time_(0),
	hitPreEffect_(0),
	hitPostEffect_(0),
	hitContinueTime_(0)
{
}

Entity& FlyingEffectComponent::GetSource() const
{
	return source_;
}

Entity& FlyingEffectComponent::GetTarget() const
{
	return target_;
}

std::string FlyingEffectComponent::GetFilename() const
{
	return filename_;
}

float FlyingEffectComponent::GetTime() const
{
	return time_;
}

void FlyingEffectComponent::SetTime(float value)
{
	time_ = value;
}

std::queue<sf::Vector2f>& FlyingEffectComponent::GetQueue()
{
	return queue_;
}

unsigned int FlyingEffectComponent::GetHitPreEffect() const
{
	return hitPreEffect_;
}

void FlyingEffectComponent::SetHitPreEffect(const unsigned int value)
{
	hitPreEffect_ = value;
}

unsigned int FlyingEffectComponent::GetHitPostEffect() const
{
	return hitPostEffect_;
}

void FlyingEffectComponent::SetHitPostEffect(const unsigned int value)
{
	hitPostEffect_ = value;
}

unsigned int FlyingEffectComponent::GetHitContinueTime() const
{
	return hitContinueTime_;
}

void FlyingEffectComponent::SetHitContinueTime(const unsigned int value)
{
	hitContinueTime_ = value;
}