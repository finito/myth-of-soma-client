
#include "Components/EffectComponent.h"

using namespace artemis;

EffectComponent::EffectComponent(Entity& parent, EffectType::Enum type, const std::string& filename, const unsigned int id, const bool lockToParent) :
	parent_(parent),
	type_(type),
	filename_(filename),
	id_(id),
	isContinueEffect_(false),
	isLockedToParent_(lockToParent)
{
}

Entity& EffectComponent::GetParent() const
{
	return parent_;
}

EffectType::Enum EffectComponent::GetType() const
{
	return type_;
}

std::string EffectComponent::GetFilename() const
{
	return filename_;
}

unsigned int EffectComponent::GetId() const
{
	return id_;
}

bool EffectComponent::IsContinueEffect() const
{
	return isContinueEffect_;
}

void EffectComponent::SetIsContinueEffect(const bool value)
{
	isContinueEffect_ = value;
}

bool EffectComponent::IsLockedToParent() const
{
	return isLockedToParent_;
}