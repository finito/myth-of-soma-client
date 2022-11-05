
#include "Systems/DeathSystem.h"

#include "Components/DeadComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/ExpiresComponent.h"

using namespace artemis;

const float DeathSystem::EntityExpireDelay = 5.0f;

DeathSystem::DeathSystem()
{
	addComponentType<DeadComponent>();
}

void DeathSystem::initialize()
{
	deadMapper.init(*world);
	attributeMapper.init(*world);
}

void DeathSystem::processEntity(Entity& e)
{
	DeadComponent* deadComponent = deadMapper.get(e);
	deadComponent->ReduceDelayTime(world->getDelta());
	if (deadComponent->IsExpired())
	{
		AttributeComponent* attributeComponent = attributeMapper.get(e);
		if (attributeComponent && !attributeComponent->IsAlive())
		{
			attributeComponent->SetState(StateType::Dead);

			if (attributeComponent->GetServerId() >= 10000)
			{
				e.addComponent(new ExpiresComponent(EntityExpireDelay));
				e.refresh();
			}
		}

		e.removeComponent<DeadComponent>();
		e.refresh();
	}
}