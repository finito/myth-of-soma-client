
#include "Systems/ExpirationSystem.h"

#include "Components/ExpiresComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/EffectComponent.h"

#include "Zone.h"

using namespace artemis;

ExpirationSystem::ExpirationSystem(Zone& zone) :
	zone_(zone)
{
	addComponentType<ExpiresComponent>();
}

void ExpirationSystem::initialize()
{
	expiresMapper.init(*world);
	effectMapper.init(*world);
	attributeMapper.init(*world);
}

void ExpirationSystem::processEntity(Entity& e)
{
	ExpiresComponent* expiresComponent = expiresMapper.get(e);
	expiresComponent->ReduceLifeTime(world->getDelta());
	if (expiresComponent->IsExpired())
	{
		AttributeComponent* attributeComponent = attributeMapper.get(e);
		if (attributeComponent != nullptr && attributeComponent->GetServerId() >= 10000)
		{
			zone_.RemoveServerEntity(attributeComponent->GetServerId());
		}

		EffectComponent* effectComponent = effectMapper.get(e);
		if (effectComponent != nullptr)
		{
			AttributeComponent* attributeComponent = attributeMapper.get(effectComponent->GetParent());
			if (attributeComponent != nullptr)
			{
				auto it = attributeComponent->GetContinueEffectEntities().find(effectComponent->GetId());
				if (it != attributeComponent->GetContinueEffectEntities().end())
				{
					attributeComponent->GetContinueEffectEntities().erase(it);
				}
			}
		}

		world->deleteEntity(e);
	}
}