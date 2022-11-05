
#include "Systems/EffectSystem.h"

#include "Components/EffectComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/PositionComponent.h"

using namespace artemis;

EffectSystem::EffectSystem()
{
	addComponentType<EffectComponent>();
}

void EffectSystem::initialize()
{
	effectMapper.init(*world);
	attributeMapper.init(*world);
	positionMapper.init(*world);
}

void EffectSystem::processEntity(Entity& e)
{
	EffectComponent* effectComponent = effectMapper.get(e);
	if (effectComponent->IsContinueEffect())
	{
		AttributeComponent* parentAttributeComponent = attributeMapper.get(effectComponent->GetParent());
		if (parentAttributeComponent != nullptr && !parentAttributeComponent->IsAlive())
		{
			world->deleteEntity(e);
			return;
		}
	}

	if (effectComponent->IsLockedToParent())
	{
		PositionComponent* parentPositionComponent = positionMapper.get(effectComponent->GetParent());
		if (parentPositionComponent)
		{
			PositionComponent* positionComponent = positionMapper.get(e);
			positionComponent->SetPosition(parentPositionComponent->GetPosition());
		}
	}
}