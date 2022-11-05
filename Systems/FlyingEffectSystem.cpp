
#include "Systems/FlyingEffectSystem.h"

#include "Components/FlyingEffectComponent.h"
#include "Components/PositionComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/ExpiresComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/EffectComponent.h"

#include "Framework/Assets/ResourceCache.h"

#include "EntityFactory.h"

using namespace artemis;

FlyingEffectSystem::FlyingEffectSystem()
{
	addComponentType<FlyingEffectComponent>();
	addComponentType<PositionComponent>();
}

void FlyingEffectSystem::initialize()
{
	flyingEffectMapper.init(*world);
	positionMapper.init(*world);
	attributeMapper.init(*world);
	expiresMapper.init(*world);
	animationMapper.init(*world);
	effectMapper.init(*world);
}

void FlyingEffectSystem::processEntity(Entity& e)
{
	flyingEffectComponent_ = flyingEffectMapper.get(e);
	flyingEffectComponent_->SetTime(flyingEffectComponent_->GetTime() + world->getDelta());
	if (flyingEffectComponent_->GetTime() >= 0.08f)
	{
		flyingEffectComponent_->SetTime(0);

		std::queue<sf::Vector2f>& queue = flyingEffectComponent_->GetQueue();
		if (queue.empty())
		{
			if (flyingEffectComponent_->GetHitPreEffect() != 0)
			{
				CreateHitEffectEntity(flyingEffectComponent_->GetHitPreEffect(), EffectType::Pre);
			}

			if (flyingEffectComponent_->GetHitPostEffect() != 0)
			{
				CreateHitEffectEntity(flyingEffectComponent_->GetHitPostEffect(), EffectType::Post);
			}
			world->deleteEntity(e);
		}
		else
		{
			positionMapper.get(e)->SetPosition(flyingEffectComponent_->GetQueue().front());
			flyingEffectComponent_->GetQueue().pop();
		}
	}
}

void FlyingEffectSystem::added(artemis::Entity& e)
{
	if (!SetRoute(e))
	{
		world->deleteEntity(e);
	}
}

bool FlyingEffectSystem::SetRoute(artemis::Entity& e)
{
	flyingEffectComponent_ = flyingEffectMapper.get(e);

	const sf::Vector2f& startPosition = positionMapper.get(flyingEffectComponent_->GetSource())->GetPosition();
	const sf::Vector2f& endPosition = positionMapper.get(flyingEffectComponent_->GetTarget())->GetPosition();

	const float distanceX = endPosition.x - startPosition.x;
	const float distanceY = endPosition.y - startPosition.y;

	const float distance = std::abs(distanceX) + std::abs(distanceY);
	const int step = static_cast<int>(distance / 80.0f + 0.5f);
	if (step == 0)
	{
		return false;
	}

	for (int i = 0; i <= step; ++i)
	{
		sf::Vector2f position(
			startPosition.x + static_cast<int>(distanceX * i / step),
			startPosition.y + static_cast<int>(distanceY * i / step));
		flyingEffectComponent_->GetQueue().push(position);
	}

	return true;
}

void FlyingEffectSystem::CreateHitEffectEntity(const unsigned int effectId, const EffectType::Enum type)
{
	AttributeComponent* attributeComponent = attributeMapper.get(flyingEffectComponent_->GetTarget());
	if (attributeComponent == nullptr)
	{
		return;
	}

	unsigned int lifeTime = flyingEffectComponent_->GetHitContinueTime();

	bool isContinueEffect = false;

	// Attempt to retrieve existing effect entity for those effects that continue for a period of time
	// and update it instead of creating a new.
	if (lifeTime != 0)
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

	Entity& effectEntity = EntityFactory::CreateEffect(*world, flyingEffectComponent_->GetTarget(), type,
		boost::str(boost::format("magicres/mgr_%1$03i.dat") % effectId), effectId);

	EffectComponent* effectComponent = effectMapper.get(effectEntity);
	effectComponent->SetIsContinueEffect(isContinueEffect);

	AnimationComponent* animationComponent = animationMapper.get(effectEntity);
	if (animationComponent != nullptr)
	{
		animationComponent->direction = 0;
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