
#include "Components/InventoryComponent.h"
#include "Network/GamePacket.h"
#include "MagicData.h"
#include "SpecialAttackData.h"
#include "AbilityData.h"

using namespace artemis;

InventoryComponent::InventoryComponent()
{
}

void InventoryComponent::AddMagic(Network::GamePacket& packet)
{
	MagicDataPtr magic(new MagicData());
	magic->HandleReceive(packet);
	
	magicMap_[magic->id_] = magic;

	switch (magic->type_)
	{
	case MagicType::Blue:
		blueMagic_.push_back(magic);
		break;
	case MagicType::White:
		whiteMagic_.push_back(magic);
		break;
	case MagicType::Black:
		blackMagic_.push_back(magic);
		break;
	}
}

void InventoryComponent::ClearMagic()
{
	MagicMap().swap(magicMap_);
	BlueMagic().swap(blueMagic_);
	WhiteMagic().swap(whiteMagic_);
	BlackMagic().swap(blackMagic_);
}

void InventoryComponent::AddSpecialAttack(Network::GamePacket& packet)
{
	SpecialAttackDataPtr specialAttack(new SpecialAttackData());
	specialAttack->HandleReceive(packet);
	
	specialAttackMap_[specialAttack->id_] = specialAttack;
	specialAttack_.push_back(specialAttack);
}

void InventoryComponent::ClearSpecialAttack()
{
	SpecialAttackMap().swap(specialAttackMap_);
	SpecialAttack().swap(specialAttack_);
}

void InventoryComponent::AddAbility(Network::GamePacket& packet)
{
	AbilityDataPtr ability(new AbilityData());
	ability->HandleReceive(packet);

	abilityMap_[ability->id_] = ability;
	ability_.push_back(ability);
}

void InventoryComponent::ClearAbility()
{
	AbilityMap().swap(abilityMap_);
	Ability().swap(ability_);
}

ItemAttributeComponent& InventoryComponent::GetInventoryItem(int index)
{
	return inventoryItem_[index];
}

std::size_t InventoryComponent::GetInventoryItemSize() const
{
	return inventoryItem_.size();
}

ItemAttributeComponent& InventoryComponent::GetBeltItem(int index)
{
	return beltItem_[index];
}

std::size_t InventoryComponent::GetBeltItemSize() const
{
	return beltItem_.size();
}

void InventoryComponent::ClearInventoryItem()
{
	ItemArray().swap(inventoryItem_);
}

void InventoryComponent::ClearBeltItem()
{
	ItemArray().swap(beltItem_);
}

void InventoryComponent::ResizeInventoryItem(std::size_t size)
{
	inventoryItem_.resize(size);
}

void InventoryComponent::ResizeBeltItem(std::size_t size)
{
	beltItem_.resize(size);
}

SpecialAttackDataPtr InventoryComponent::GetSpecialAttackByIndex(int index) const
{
	return specialAttack_[index];
}

SpecialAttackDataPtr InventoryComponent::GetSpecialAttackById(short id) const
{
	auto it = specialAttackMap_.find(id);
	if (it != specialAttackMap_.cend())
	{
		return it->second;
	}
	return SpecialAttackDataPtr();
}

std::size_t InventoryComponent::GetSpecialAttackSize() const
{
	 return specialAttack_.size();
}

MagicDataPtr InventoryComponent::GetMagic(short id) const
{
	auto it = magicMap_.find(id);
	if (it != magicMap_.cend())
	{
		return it->second;
	}
	return MagicDataPtr();
}

std::size_t InventoryComponent::GetMagicSize() const
{
	return magicMap_.size();
}

AbilityDataPtr InventoryComponent::GetAbility(short id) const
{
	auto it = abilityMap_.find(id);
	if (it != abilityMap_.cend())
	{
		return it->second;
	}
	return AbilityDataPtr();
}

std::size_t InventoryComponent::GetAbilitySize() const
{
	return abilityMap_.size();
}

MagicDataPtr InventoryComponent::GetBlueMagic(int index) const
{
	return blueMagic_[index];
}

std::size_t InventoryComponent::GetBlueMagicSize() const
{
	return blueMagic_.size();
}

MagicDataPtr InventoryComponent::GetWhiteMagic(int index) const
{
	return whiteMagic_[index];
}

std::size_t InventoryComponent::GetWhiteMagicSize() const
{
	return whiteMagic_.size();
}

MagicDataPtr InventoryComponent::GetBlackMagic(int index) const
{
	return blackMagic_[index];
}

std::size_t InventoryComponent::GetBlackMagicSize() const
{
	return blackMagic_.size();
}