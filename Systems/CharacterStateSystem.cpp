
#include "Systems/CharacterStateSystem.h"

#include "Components/AttributeComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/AnimationComponent.h"

#include "DataTables/ItemTable.h"
#include "Game.h"

#include "Framework/Assets/ResourceCache.h"

#include <boost/format.hpp>

using namespace artemis;

CharacterStateSystem::CharacterStateSystem()
{
	addComponentType<AttributeComponent>();
	addComponentType<AnimationComponent>();
}

void CharacterStateSystem::initialize()
{
	attributeMapper.init(*world);
	equipmentMapper.init(*world);
	animationMapper.init(*world);
}

void CharacterStateSystem::processEntity(Entity& e)
{
	attributeComponent_ = attributeMapper.get(e);
	equipmentComponent_ = equipmentMapper.get(e);
	animationComponent_ = animationMapper.get(e);

	if (animationComponent_->animation && !animationComponent_->loop && animationComponent_->finished)
	{
		if (attributeComponent_->GetState() != StateType::Dead)
		{
			attributeComponent_->SetState(StateType::Idle);
		}
	}

	SetAnimation();
}

void CharacterStateSystem::SetAnimation()
{
	const unsigned int animationState = GetAnimationState();
	if (animationComponent_->animation == nullptr || animationComponent_->animationState != animationState)
	{
		switch (attributeComponent_->GetState())
		{
		case StateType::Idle:
		case StateType::Walking:
		case StateType::Running:
			animationComponent_->loop = true;
			break;
		default:
			animationComponent_->loop = false;
			break;
		}
		animationComponent_->animationState = animationState;
		animationComponent_->time = 0;
		unsigned int pictureId = attributeComponent_->GetServerId() < 10000 ? 0 : attributeComponent_->GetSkin();
		if (attributeComponent_->GetGender() == Gender::Female) pictureId += 3;
		animationComponent_->animation = Framework::animationCache.Get(
			boost::str(boost::format("man/man%1$03i%2$02i.ani") % pictureId % animationComponent_->animationState));
	}
	animationComponent_->direction = attributeComponent_->GetDirection();
}

unsigned int CharacterStateSystem::GetAnimationState() const
{
	WeaponType::Enum weaponType = WeaponType::PunchOrKnuckle;
	if (equipmentComponent_ && equipmentComponent_->pictureId[ArmatureType::Weapon] != 0)
	{
		const ItemTable::ItemElement* itemElement = Game::Instance().itemTable->Get(equipmentComponent_->pictureId[ArmatureType::Weapon]);
		weaponType = static_cast<WeaponType::Enum>(itemElement->equipPosition);
	}

	switch (attributeComponent_->GetState())
	{
	case StateType::Idle:
		if (attributeComponent_->GetBattleMode() == BattleMode::Normal)
		{
			return attributeComponent_->GetServerId() < 10000 ? 18 : 0;
		}
		else
		{
			switch (weaponType)
			{
			case WeaponType::PunchOrKnuckle:
			case WeaponType::Sword:
			case WeaponType::Bow:
			case WeaponType::Staff:
			case WeaponType::CrossBow:
			case WeaponType::Axe:
				return 0;
			case WeaponType::BigSword:
			case WeaponType::BigAxe:
			case WeaponType::Spear:
				return 1;
			}
		}
		break;
	case StateType::Walking:
		if (attributeComponent_->GetBattleMode() == BattleMode::Normal)
		{
			return 4;
		}

		switch (weaponType)
		{
		case WeaponType::PunchOrKnuckle:
		case WeaponType::Sword:
		case WeaponType::Bow:
		case WeaponType::Staff:
		case WeaponType::CrossBow:
		case WeaponType::Axe:
			return 4;
		case WeaponType::BigSword:
		case WeaponType::BigAxe:
		case WeaponType::Spear:
			return 5;
		}
		break;
	case StateType::Attacking:
		if (attributeComponent_->GetBattleMode() == BattleMode::Normal)
		{
			return 7;
		}

		switch (weaponType)
		{
		case WeaponType::PunchOrKnuckle:
			return 7;
		case WeaponType::Sword:
		case WeaponType::Staff:
		case WeaponType::Axe:
			return 8;
		case WeaponType::Spear:
			return 10;
		case WeaponType::Bow:
			return 11;
		case WeaponType::CrossBow:
			return 6;
		}

		if (attributeComponent_->GetGender() == Gender::Male)
		{
			if (weaponType == WeaponType::BigSword) return 9;
			if (weaponType == WeaponType::BigAxe) return 12;
		}
		else
		{
			if (weaponType == WeaponType::BigSword || weaponType == WeaponType::BigAxe) return 12;
		}
		break;
	case StateType::Hit:
		return 13;
	case StateType::Cast:
		return 14;
	case StateType::Dead:
		return 15;
	case StateType::Running:
		return 16;
	default:
		assert(0 && "Character is in invalid animation state.");
		break;
	}
	return 0;
}