#include "Components/ArrowComponent.h"

using namespace artemis;

ArrowComponent::ArrowComponent(Entity& parent, Entity& target, int type, bool hit) :
	parent_(parent),
	target_(target),
	type_(type),
	time_(0),
	hit_(hit)
{
}

int ArrowComponent::GetType() const
{
	return type_;
}

void ArrowComponent::SetType(int value)
{
	type_ = value;
}

float ArrowComponent::GetTime() const
{
	return time_;
}

void ArrowComponent::SetTime(float value)
{
	time_ = value;
}

int ArrowComponent::GetDirection() const
{
	return direction_;
}

void ArrowComponent::SetDirection(int value)
{
	direction_ = value;
}

Entity& ArrowComponent::GetParent() const
{
	return parent_;
}

Entity& ArrowComponent::GetTarget() const
{
	return target_;
}

std::queue<sf::Vector2f>& ArrowComponent::GetQueue()
{
	return queue_;
}

bool ArrowComponent::IsHit()
{
	return hit_;
}