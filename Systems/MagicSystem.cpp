
#include "Systems/MagicSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/FocusSystem.h"

#include "Components/PositionComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/ExpiresComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/FlyingEffectComponent.h"

#include "Artemis/SystemManager.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include "Framework/Assets/ResourceCache.h"

#include "Game.h"
#include "EntityFactory.h"
#include "DataTables/MagicTable.h"
#include "Zone.h"

#include <boost/format.hpp>

using namespace artemis;

MagicSystem::MagicSystem(Zone& zone) :
	zone_(zone)
{
	addComponentType<MagicComponent>();
}

void MagicSystem::initialize()
{
	focusSystem_ = dynamic_cast<FocusSystem*>(world->getSystemManager()->getSystem<FocusSystem>());

	magicMapper.init(*world);
	effectMapper.init(*world);
	positionMapper.init(*world);
	animationMapper.init(*world);
	expiresMapper.init(*world);
	attributeMapper.init(*world);
	flyingEffectMapper.init(*world);
}

void MagicSystem::processEntity(Entity& e)
{
	MagicComponent* magicComponent = magicMapper.get(e);
	MagicTable::MagicElement& magicTable = magicComponent->GetMagicTable();
	if (magicTable.startTime > 0)
	{
		if (magicTable.startPreMagic != 0)
		{
			CreateEffectEntity(magicTable, magicComponent->GetSource(), magicTable.startPreMagic, EffectType::Pre, magicTable.startTime);
			magicTable.startPreMagic = 0;
		}

		if (magicTable.startPostMagic != 0)
		{
			CreateEffectEntity(magicTable, magicComponent->GetSource(), magicTable.startPostMagic, EffectType::Post, magicTable.startTime);
			magicTable.startPostMagic = 0;
		}

		magicTable.startTime = std::max(0, magicTable.startTime - static_cast<int>(world->getDelta() * 1000.0f));
	}
	else
	{
		if (!magicComponent->HasAttackSent() && magicComponent->GetSource().getId() == Game::Instance().myEntity->getId())
		{
			if (!PlayerMagicAttack(*magicComponent))
			{
				world->deleteEntity(e);
				return;
			}
			magicComponent->SetAttackSent(true);
		}

		if (magicComponent->IsReady())
		{
			AttributeComponent* attributeComponent = attributeMapper.get(magicComponent->GetSource());
			if (attributeComponent != nullptr)
			{
				if (magicComponent->GetMagicTable().flyMagic != 0)
				{
					attributeComponent->SetState(StateType::Attacking);
				}
				else
				{
					if (attributeComponent->GetServerId() < 10000)
					{
						attributeComponent->SetState(StateType::Cast);
					}
					else
					{
						attributeComponent->SetState(StateType::Attacking);
					}
				}
			}

			std::vector<Entity*>& targets = magicComponent->GetTargets();
			for (auto it = targets.begin(); it != targets.end(); ++it)
			{
				Entity* target = (*it);
				if (target != nullptr)
				{
					if (magicTable.flyMagic != 0)
					{
						FlyingEffect(*magicComponent, *target);
					}
					else
					{
						if (magicTable.endPreMagic != 0)
						{
							CreateEffectEntity(magicTable, *target, magicTable.endPreMagic, EffectType::Pre, magicTable.continueTime);
						}

						if (magicTable.endPostMagic != 0)
						{
							CreateEffectEntity(magicTable, *target, magicTable.endPostMagic, EffectType::Post, magicTable.continueTime);
						}
					}
				}
			}
			world->deleteEntity(e);
			return;
		}
	}
}

void MagicSystem::added(Entity& e)
{

}

void MagicSystem::removed(Entity& e)
{
	std::cout << "Magic Entity Removed" << std::endl;
	MagicComponent* magicComponent = magicMapper.get(e);
	if (magicComponent != nullptr)
	{
		AttributeComponent* attributeComponent = attributeMapper.get(magicComponent->GetSource());
		if (attributeComponent != nullptr)
		{
			attributeComponent->GetMagicEntityMap().erase(magicComponent->GetMagicTable().magicId);
		}
	}
}

void MagicSystem::CreateEffectEntity(const MagicTable::MagicElement& magicTable, Entity& parent, unsigned int effectId,
	EffectType::Enum type, unsigned int lifeTime)
{
	AttributeComponent* attributeComponent = attributeMapper.get(parent);
	if (attributeComponent == nullptr)
	{
		return;
	}

	bool isContinueEffect = false;

	// Attempt to retrieve existing effect entity for those effects that continue for a period of time
	// and update it instead of creating a new.
	if (lifeTime != 0 &&
		effectId == magicTable.endPreMagic || effectId == magicTable.endPostMagic)
	{
		isContinueEffect = true;

		auto it = attributeComponent->GetContinueEffectEntities().find(effectId);
		if (it != attributeComponent->GetContinueEffectEntities().end())
		{
			ExpiresComponent* expiresComponent = expiresMapper.get(it->second);
			if (expiresComponent != nullptr)
			{
				expiresComponent->SetLifeTime(static_cast<float>(lifeTime) / 1000.0f);
				return;
			}
		}
	}

	Entity& effectEntity = EntityFactory::CreateEffect(*world, parent, type,
		boost::str(boost::format("magicres/mgr_%1$03i.dat") % effectId), effectId);

	EffectComponent* effectComponent = effectMapper.get(effectEntity);
	effectComponent->SetIsContinueEffect(isContinueEffect);

	AnimationComponent* animationComponent = animationMapper.get(effectEntity);
	if (animationComponent != nullptr)
	{
		if (effectId == magicTable.flyMagic)
		{
			animationComponent->direction = attributeComponent->GetDirection();
		}
		else
		{
			animationComponent->direction = 0;
		}
		
		animationComponent->animation = Framework::animationCache.Get(
			boost::str(boost::format("magicres/mgr_%1$03i.ani") % effectId));
		if (lifeTime == 0)
		{
			lifeTime = static_cast<unsigned int>(animationComponent->animation->GetLength(0) * 1000.0f);
		}
		else
		{
			animationComponent->loop = true;
		}
	}

	effectEntity.addComponent(new ExpiresComponent(static_cast<float>(lifeTime) / 1000.0f));
	effectEntity.refresh();

	if (isContinueEffect)
	{
		attributeComponent->GetContinueEffectEntities().insert(
			std::pair<unsigned short, Entity&>(effectId, effectEntity));
	}
}

void MagicSystem::FlyingEffect(const MagicComponent& magicComponent, Entity& target)
{
	AttributeComponent* attributeComponent = attributeMapper.get(magicComponent.GetSource());
	if (attributeComponent == nullptr)
	{
		return;
	}

	Entity& effectEntity = EntityFactory::CreateFlyingEffect(*world, magicComponent.GetSource(), target,
		boost::str(boost::format("magicres/mgr_%1$03i.dat") % magicComponent.GetMagicTable().flyMagic));

	FlyingEffectComponent* flyingEffectComponent = flyingEffectMapper.get(effectEntity);
	if (flyingEffectComponent != nullptr)
	{
		flyingEffectComponent->SetHitPreEffect(magicComponent.GetMagicTable().endPreMagic);
		flyingEffectComponent->SetHitPostEffect(magicComponent.GetMagicTable().endPostMagic);
		flyingEffectComponent->SetHitContinueTime(magicComponent.GetMagicTable().continueTime);
	}

	AnimationComponent* animationComponent = animationMapper.get(effectEntity);
	if (animationComponent != nullptr)
	{
		animationComponent->direction = magicComponent.GetDirection();
		animationComponent->animation = Framework::animationCache.Get(
			boost::str(boost::format("magicres/mgr_%1$03i.ani") % magicComponent.GetMagicTable().flyMagic));
		std::cout << "AnimationComponent direction=" << static_cast<int>(animationComponent->direction) << std::endl;
	}
}

artemis::Entity* MagicSystem::GetMagicEntity(Entity& e, unsigned short magicId)
{
	AttributeComponent* attributeComponent = attributeMapper.get(e);
	if (attributeComponent == nullptr)
	{
		return nullptr;
	}

	auto it = attributeComponent->GetMagicEntityMap().find(magicId);
	if (it == attributeComponent->GetMagicEntityMap().end())
	{
		return nullptr;
	}

	Entity* magic = it->second;
	if (magic != nullptr)
	{
		MagicComponent* magicComponent = magicMapper.get(*magic);
		if (magicComponent->GetMagicTable().magicId == magicId)
		{
			return magic;
		}
	}

	return nullptr;
}

bool MagicSystem::PlayerNotInValidState(const StateType::Enum state, const BattleMode::Enum battleMode) const
{
	return state == StateType::Attacking || state == StateType::Walking ||
		state == StateType::Running || battleMode == BattleMode::Normal
		? true
		: false;
}

bool MagicSystem::PlayerIsCastingMagic(const std::map<unsigned short, Entity*>& entities)
{
	for (auto it = entities.cbegin(); it != entities.cend(); ++it)
	{
		MagicComponent* magicComponent = magicMapper.get(*it->second);
		assert (magicComponent != nullptr && "Entity is invalid (must have magic component)");
		if (magicComponent->GetMagicTable().startTime != 0)
		{
			return true;
		}
	}
	return false;
}

sf::Vector2i MagicSystem::GetEntityCellPosition(Entity& e)
{
	PositionComponent* positionComponent = positionMapper.get(e);
	assert (positionComponent != nullptr && "Entity is invalid (must have position component)");
	return positionComponent->GetCellPosition();
}

Direction::Enum MagicSystem::GetDirectionToFace(const int targetServerId, const Direction::Enum initialValue)
{
	return targetServerId == -1
		? initialValue
		: MovementSystem::GetDirection(
		GetEntityCellPosition(*Game::Instance().myEntity),
		GetEntityCellPosition(*zone_.GetServerEntity(targetServerId)));
}

int MagicSystem::GetTargetServerId(const short method, const int playerServerId) const
{
	int result = focusSystem_->GetCharacterFocusServerId();
	if (result == -1 && method == 1)
	{
		result = playerServerId;
	}
	return result;
}

bool MagicSystem::TargetIsValid(const int targetServerId, const short method, const int targetType, const int playerServerId) const
{
	if (method == 1)
	{
		if (targetServerId == playerServerId && targetType != 1 && targetType != 2)
		{
			return false;
		}

		if (targetServerId > 10000 && targetType != 3)
		{
			return false;
		}
	}
	return true;
}

void MagicSystem::SendMagicCast(const int targetServerId, const unsigned short magicId, const unsigned short direction)
{
	Network::GamePacket packet(Network::GameProtocol::PKT_MAGIC_READY);
	packet << targetServerId << magicId << static_cast<unsigned char>((direction + 1) % 8) << direction;
	Game::Instance().gameSocket->Send(packet);
}

void MagicSystem::SendMagicAttack(const int targetServerId, const unsigned short magicId, const unsigned short direction)
{
	Network::GamePacket packet(Network::GameProtocol::PKT_ATTACK_MAGIC_ARROW);
	packet << direction << targetServerId << magicId;
	Game::Instance().gameSocket->Send(packet);
}

bool MagicSystem::PlayerMagicCast(const unsigned short magicId)
{
	if (magicId == 0)
	{
		return false;
	}

	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent == nullptr)
	{
		return false;
	}

	if (PlayerNotInValidState(attributeComponent->GetState(), attributeComponent->GetBattleMode()) ||
		PlayerIsCastingMagic(attributeComponent->GetMagicEntityMap()))
	{
		return false;
	}

	const MagicTable::MagicElement* magicTable = Game::Instance().magicTable->Get(magicId);
	if (magicTable == nullptr)
	{
		return false;
	}

	const int targetServerId = GetTargetServerId(magicTable->method, attributeComponent->GetServerId());
	if (!TargetIsValid(targetServerId, magicTable->method, magicTable->targetType, attributeComponent->GetServerId()))
	{
		return false;
	}

	unsigned short direction = static_cast<unsigned short>(GetDirectionToFace(targetServerId, attributeComponent->GetDirection()));
	SendMagicCast(targetServerId, magicId, direction);
	return true;
}

bool MagicSystem::PlayerMagicAttack(const MagicComponent& magicComponent)
{
	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent == nullptr)
	{
		return false;
	}

	if (PlayerNotInValidState(attributeComponent->GetState(), attributeComponent->GetBattleMode()) ||
		PlayerIsCastingMagic(attributeComponent->GetMagicEntityMap()))
	{
		return false;
	}

	switch (magicComponent.GetMagicTable().method)
	{
	case 1:
		return PlayerMagicAttackArrow(magicComponent, *attributeComponent);
	default:
		return false;
	}
}

bool MagicSystem::PlayerMagicAttackArrow(const MagicComponent& magicComponent, AttributeComponent& attributeComponent)
{
	int targetServerId = -1;
	if (!magicComponent.GetTargets().empty())
	{
		AttributeComponent* targetAttributeComponent = attributeMapper.get(*magicComponent.GetTargets().at(0));
		if (targetAttributeComponent != nullptr)
		{
			targetServerId = targetAttributeComponent->GetServerId();
		}
	}
	else
	{
		targetServerId = GetTargetServerId(magicComponent.GetMagicTable().method, attributeComponent.GetServerId());
		if (!TargetIsValid(targetServerId, magicComponent.GetMagicTable().method, magicComponent.GetMagicTable().targetType,
			attributeComponent.GetServerId()))
		{
			return false;
		}
	}

	if (targetServerId == -1)
	{
		return false;
	}

	unsigned short direction = static_cast<unsigned short>(GetDirectionToFace(targetServerId, attributeComponent.GetDirection()));
	SendMagicAttack(targetServerId, magicComponent.GetMagicTable().magicId, direction);
	attributeComponent.SetState(StateType::Idle);
	return true;
}