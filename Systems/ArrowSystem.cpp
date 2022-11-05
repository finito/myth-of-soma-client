
#include "Systems/ArrowSystem.h"
#include "Systems/CameraSystem.h"

#include "Components/ArrowComponent.h"
#include "Components/PositionComponent.h"
#include "Components/AreaComponent.h"
#include "Components/AttributeComponent.h"

#include "Artemis/SystemManager.h"

#include "Framework/Assets/ResourceCache.h"

#include "EntityFactory.h"

#include <cmath>

using namespace artemis;

ArrowSystem::ArrowSystem()
{
	addComponentType<ArrowComponent>();
	addComponentType<PositionComponent>();
}

void ArrowSystem::initialize()
{
	SetRadArrays();

	arrowMapper.init(*world);
	positionMapper.init(*world);
	areaMapper.init(*world);
	attributeMapper.init(*world);

    cameraSystem_ = dynamic_cast<CameraSystem*>(world->getSystemManager()->getSystem<CameraSystem>());
}

void ArrowSystem::processEntity(Entity& e)
{
	ArrowComponent* arrowComponent = arrowMapper.get(e);
	arrowComponent->SetTime(arrowComponent->GetTime() + world->getDelta());
	if (arrowComponent->GetTime() >= 0.05f)
	{
		arrowComponent->SetTime(0);

		std::queue<sf::Vector2f>& queue = arrowComponent->GetQueue();
		if (queue.empty())
		{
			if (arrowComponent->IsHit())
			{
				Entity& target = arrowComponent->GetTarget();
				AttributeComponent* targetAttributeComponent = attributeMapper.get(target);
				if (targetAttributeComponent != nullptr)
				{
					targetAttributeComponent->SetState(StateType::Hit);
					EntityFactory::CreateBlood(*world, target, targetAttributeComponent->GetBloodType(), true);
				}
			}

			world->deleteEntity(e);
		}
		else
		{
			positionMapper.get(e)->SetPosition(arrowComponent->GetQueue().front());
			arrowComponent->GetQueue().pop();
		}
	}
}

void ArrowSystem::added(artemis::Entity& e)
{
	if (!SetArrowRoute(e))
	{
		world->deleteEntity(e);
	}
}

void ArrowSystem::SetRadArrays()
{
	double pi = 3.14159265358;
	double radDegree = pi / 180;
	double gapDegree = 360.0 / MAX_DIRECTION;
	for (int i = 0; i < MAX_DIRECTION; ++i)
	{
		yrad[i]	   = -cos(gapDegree * i * radDegree);
		xrad[i]	   =  sin(gapDegree * i * radDegree);
		ygaprad[i] = -cos((gapDegree * i - gapDegree / 2 ) * radDegree);
		xgaprad[i] =  sin((gapDegree * i - gapDegree / 2 ) * radDegree);
	}
}

int ArrowSystem::GetDirection(artemis::Entity& e, const double distanceX, const double distanceY)
{
	if( distanceX >= 0 )
	{
		if (distanceY <= ygaprad[1])
		{
			return 0;
		}

		if (distanceY >= ygaprad[MAX_DIRECTION / 2])
		{
			return MAX_DIRECTION / 2;
		}

		for (int i = 0; i < MAX_DIRECTION / 2; ++i)
		{
			if (distanceY >= ygaprad[i] && distanceY < ygaprad[i + 1])
			{
				return i;
			}
		}
	}
	else if (distanceX <= 0)
	{
		if (distanceY >= ygaprad[MAX_DIRECTION/2])
		{
			return MAX_DIRECTION / 2;
		}
		
		if (distanceY <= ygaprad[0])
		{
			return 0;
		}

		for (int i = MAX_DIRECTION / 2; i < MAX_DIRECTION; ++i)
		{
			int ii = ((i + 1) == MAX_DIRECTION) ? 0 : (i + 1);
			if (distanceY >= ygaprad[ii] && distanceY < ygaprad[i])
			{
				return i;
			}
		}
	}

	return -1;
}

bool ArrowSystem::SetArrowRoute(artemis::Entity& e)
{
	ArrowComponent* arrowComponent = arrowMapper.get(e);

	sf::Vector2f& startPosition = positionMapper.get(arrowComponent->GetParent())->GetPosition();
	startPosition.y -= 50;

	sf::Vector2f& endPosition = positionMapper.get(arrowComponent->GetTarget())->GetPosition();
	endPosition.y -= areaMapper.get(arrowComponent->GetTarget())->GetArea().height / 2;

	double distance = std::sqrt(
		(endPosition.x - startPosition.x) * (endPosition.x - startPosition.x) +
		(endPosition.y - startPosition.y) * (endPosition.y - startPosition.y));
	if (distance <= 0)
	{
		return false;
	}

	const double distanceX = static_cast<double>(endPosition.x - startPosition.x) / distance;
	const double distanceY = static_cast<double>(endPosition.y - startPosition.y) / distance;

	int direction = GetDirection(e, distanceX, distanceY);
	if (direction == -1)
	{
		return false;
	}

	arrowComponent->SetDirection(direction);

	if (arrowComponent->IsHit())
	{
		int i = 0;
		for (;;)
		{
			const sf::Vector2f position(
				startPosition.x + static_cast<int>(distanceX * MOVE_RATE * i),
				startPosition.y + static_cast<int>(distanceY * MOVE_RATE * i));

			distance = std::sqrt(
				(endPosition.x - position.x) * (endPosition.x - position.x) +
				(endPosition.y - position.y) * (endPosition.y - position.y));

			if (distance < 20)
			{
				break;
			}

			arrowComponent->GetQueue().push(position);
			++i;
		}
	}
	else
	{
		int i = 0;
		for (;;)
		{
			const sf::Vector2f position(
				startPosition.x + static_cast<int>(xrad[direction] * MOVE_RATE * i),
				startPosition.y + static_cast<int>(yrad[direction] * MOVE_RATE * i));

			if ((xrad[direction] < 0 && position.x < 0) || (xrad[direction] > 0 && position.x > cameraSystem_->GetX() + cameraSystem_->GetWidth()))
			{
				break;
			}

			if ((yrad[direction] < 0 && position.y < 0) || (yrad[direction] > 0 && position.y > cameraSystem_->GetY() + cameraSystem_->GetHeight()))
			{
				break;
			}

			arrowComponent->GetQueue().push(position);
			++i;
		}
	}

	return true;
}