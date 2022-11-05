
#include "Components/MagicComponent.h"

using namespace artemis;

MagicComponent::MagicComponent(Entity& source, const MagicTable::MagicElement& magicTable) :
	source_(source),
	magicTable_(magicTable),
	ready_(false),
	direction_(Direction::Down),
	attackSent_(false)
{
}

Entity& MagicComponent::GetSource() const
{
	return source_;
}

MagicTable::MagicElement& MagicComponent::GetMagicTable()
{
	return magicTable_;
}

const MagicTable::MagicElement& MagicComponent::GetMagicTable() const
{
	return magicTable_;
}

bool MagicComponent::IsReady() const
{
	return ready_;
}

void MagicComponent::SetReady(bool value)
{
	ready_ = value;
}

Direction::Enum MagicComponent::GetDirection() const
{
	return direction_;
}

void MagicComponent::SetDirection(Direction::Enum value)
{
	direction_ = value;
}

void MagicComponent::SetDirection(unsigned char value)
{
	// FIX !Assumes that the cast will always be valid!
	direction_ = static_cast<Direction::Enum>(value);
}

std::vector<Entity*>& MagicComponent::GetTargets()
{
	return targets_;
}

const std::vector<Entity*>& MagicComponent::GetTargets() const
{
	return targets_;
}

bool MagicComponent::HasAttackSent() const
{
	return attackSent_;
}

void MagicComponent::SetAttackSent(bool value)
{
	attackSent_ = value;
}