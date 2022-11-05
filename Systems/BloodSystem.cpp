
#include "Systems/BloodSystem.h"

#include "Components/PositionComponent.h"
#include "Components/BloodComponent.h"

using namespace artemis;

BloodSystem::BloodSystem()
{
	addComponentType<BloodComponent>();
	addComponentType<PositionComponent>();
}

void BloodSystem::initialize()
{
	bloodMapper.init(*world);
	positionMapper.init(*world);
}

void BloodSystem::processEntity(Entity& e)
{
	BloodComponent* bloodComponent = bloodMapper.get(e);
	if (bloodComponent->GetMaximumKeyFrame() == 0)
	{
		return;
	}

	bloodComponent->SetTime(bloodComponent->GetTime() + world->getDelta());

	if (bloodComponent->IsStain())
	{
		if (bloodComponent->GetTime() >= 1.0f)
		{
			bloodComponent->SetKeyFrame(bloodComponent->GetKeyFrame() + 1);
			bloodComponent->SetTime(0);
		}
	}
	else
	{
		if (bloodComponent->GetTime() >= 0.1f)
		{
			bloodComponent->SetKeyFrame(bloodComponent->GetKeyFrame() + 1);
			bloodComponent->SetTime(0);

			if (!bloodComponent->IsAttack() && bloodComponent->GetKeyFrame() >= bloodComponent->GetMaximumKeyFrame() / 2)
			{
				bloodComponent->SetKeyFrame(bloodComponent->GetKeyFrame() - 1);
				bloodComponent->SetStain(true);
			}
		}
	}

	PositionComponent* parentPositionComponent = positionMapper.get(bloodComponent->GetParent());
	if (parentPositionComponent)
	{
		PositionComponent* positionComponent = positionMapper.get(e);
		positionComponent->SetPosition(parentPositionComponent->GetPosition());
	}

	if (bloodComponent->GetKeyFrame() >= bloodComponent->GetMaximumKeyFrame())
	{
		world->deleteEntity(e);
	}
}

void BloodSystem::added(Entity& e)
{
	bloodMap_[bloodMapper.get(e)->GetParent().getId()].push_back(e.getId());
}

void BloodSystem::removed(Entity& e)
{
	auto it = bloodMap_.find(bloodMapper.get(e)->GetParent().getId());
	if (it != bloodMap_.end())
	{
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it2)
		{
			if ((*it2) == e.getId())
			{
				(*it).second.erase(it2);
				break;
			}
		}
	}
}

void BloodSystem::GetBloodForEntity(Entity& e, std::vector<int>& bloodList) const
{
	auto it = bloodMap_.find(e.getId());
	if (it != bloodMap_.end())
	{
		bloodList = (*it).second;
	}
}