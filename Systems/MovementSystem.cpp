
#include "Systems/MovementSystem.h"

#include "Components/MovementComponent.h"
#include "Components/PositionComponent.h"

#include "Framework/Utils/PositionConvert.h"

#include "Game.h"
#include "Zone.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

using namespace artemis;

MovementSystem::MovementSystem(Zone& zone) :
zone_(zone),
elapsedTime_(0),
playerCanRecoverStamina_(false),
playerStaminaRecoveryDelay_(1.0f),
player_(nullptr)
{
	addComponentType<MovementComponent>();
	addComponentType<PositionComponent>();
	addComponentType<AttributeComponent>();
}

void MovementSystem::initialize()
{
	movementMapper.init(*world);
	positionMapper.init(*world);
	attributeMapper.init(*world);
}

void MovementSystem::processEntity(Entity& e)
{
	EnsurePlayerEntity();

	if (&e == player_)
	{
		UpdatePlayerStamina();
	}

	MovementComponent* movementComponent = movementMapper.get(e);
	std::queue<sf::Vector2i>& queue = movementComponent->GetQueue();
	if (queue.empty())
	{
		return;
	}

	AttributeComponent* attributeComponent = attributeMapper.get(e);
	if (!attributeComponent->IsAlive())
	{
		while (!queue.empty()) queue.pop();
		return;
	}

	StateType::Enum state = attributeComponent->GetState();
	if (state != StateType::Walking && state != StateType::Running)
	{
		return;
	}

	if (&e == player_)
	{
		if (attributeComponent->GetRunToggle() && attributeComponent->GetStamina() == 0)
		{
			attributeComponent->SetRunToggle(false);
			SendPlayerRunToggle(false);
			movementComponent->SetRunning(false);
		}
	}

	float moveDurationTime = state == StateType::Walking ? 0.7f : 0.38f;
	if (&e != player_ && queue.size() >= 6) moveDurationTime = 0.2f;

	PositionComponent* positionComponent = positionMapper.get(e);

	sf::Vector2i startCellPosition = movementComponent->GetStartCellPosition();
	if (startCellPosition.x == -1 && startCellPosition.y == -1)
	{
		startCellPosition = positionComponent->GetCellPosition();
		movementComponent->SetStartCellPosition(startCellPosition);
	}

	sf::Vector2i nextCellPosition = movementComponent->GetNextCellPosition();
	if (nextCellPosition.x == -1 && nextCellPosition.y == -1)
	{
		nextCellPosition = queue.front();
		movementComponent->SetNextCellPosition(nextCellPosition);
	}

	if (nextCellPosition == startCellPosition)
	{
		if (&e == player_)
		{
			if (movementComponent->GetRunning())
			{
				if (movementComponent->GetSentNextRunCellPosition().x == -1 &&
					movementComponent->GetSentNextRunCellPosition().y == -1)
				{
					if (queue.size() > 1) queue.pop();
					nextCellPosition = queue.front();
					movementComponent->SetNextCellPosition(nextCellPosition);
					if (queue.size() > 1)
					{
						queue.pop();
						movementComponent->SetSentNextRunCellPosition(queue.front());
					}
					else
					{
						movementComponent->SetSentNextRunCellPosition(sf::Vector2i(-1, -1));
					}
					SendPlayerMove();
				}
				else
				{
					movementComponent->SetNextCellPosition(movementComponent->GetSentNextRunCellPosition());
					movementComponent->SetSentNextRunCellPosition(sf::Vector2i(-1, -1));
				}
			}
			else
			{
				if (queue.size() > 1) queue.pop();
				nextCellPosition = queue.front();
				movementComponent->SetNextCellPosition(nextCellPosition);
				SendPlayerMove();
			}
		}
		else
		{
			if (queue.size() > 1) queue.pop();
			nextCellPosition = queue.front();
			movementComponent->SetNextCellPosition(nextCellPosition);
		}
	}

	const sf::Vector2f position = Utils::PositionConvert::CellToPixel(startCellPosition);
	const sf::Vector2f nextPosition = Utils::PositionConvert::CellToPixel(nextCellPosition);

	if (&e != player_)
	{
		zone_.SetMovable(startCellPosition, true);
		zone_.SetMovable(nextCellPosition, false);
	}

	movementComponent->IncreaseTime(world->getDelta());
	const float lerpFactor = std::min(1.f, movementComponent->GetTime() / moveDurationTime);

	const sf::Vector2f gap(nextPosition.x - position.x, nextPosition.y - position.y);

	positionComponent->SetPosition(sf::Vector2f(std::ceilf(position.x + (gap.x * lerpFactor)), std::ceilf(position.y + (gap.y * lerpFactor))));

	if (movementComponent->GetTime() >= moveDurationTime)
	{
		movementComponent->DecreaseTime(moveDurationTime);
		movementComponent->SetStartCellPosition(positionComponent->GetCellPosition());
		if (movementComponent->GetRunning())
		{
			attributeComponent->SetState(StateType::Running);
		}
		else
		{
			attributeComponent->SetState(StateType::Walking);
		}

		if (&e == player_)
		{
			if (movementComponent->GetSentNextRunCellPosition().x == -1 &&
				movementComponent->GetSentNextRunCellPosition().y == -1)
			{
				if (movementComponent->GetRunning())
				{
					queue.pop();
					if (!queue.empty())
					{
						movementComponent->SetNextCellPosition(queue.front());

						if (queue.size() > 1)
						{
							queue.pop();
							movementComponent->SetSentNextRunCellPosition(queue.front());
						}
						else
						{
							movementComponent->SetSentNextRunCellPosition(sf::Vector2i(-1, -1));
						}
						SendPlayerMove();
					}
				}
				else
				{
					queue.pop();
					if (!queue.empty())
					{
						movementComponent->SetNextCellPosition(queue.front());
						SendPlayerMove();
					}
				}
			}
			else
			{
				movementComponent->SetNextCellPosition(movementComponent->GetSentNextRunCellPosition());
				movementComponent->SetSentNextRunCellPosition(sf::Vector2i(-1, -1));
			}
		}
		else
		{
			queue.pop();
			if (!queue.empty())
			{
				movementComponent->SetNextCellPosition(queue.front());
			}
		}
	}

	if (gap.x == 0 && gap.y == 0)
	{
		attributeComponent->SetState(StateType::Idle);
		movementComponent->SetTime(0);
		if (&e != player_)
		{
			zone_.SetMovable(movementComponent->GetStartCellPosition(), true);
			zone_.SetMovable(positionComponent->GetCellPosition(), false);
		}
		movementComponent->SetStartCellPosition(positionComponent->GetCellPosition()); // FIX: !Is this needed?!
		if (!queue.empty())
		{		
			queue.pop();
		}
	}
	else
	{
		attributeComponent->SetDirection(GetDirection(startCellPosition, nextCellPosition));
	}

	if (positionComponent->GetPosition() == movementComponent->GetDestinationPosition())
	{
		if (queue.empty())
		{
			attributeComponent->SetState(StateType::Idle);
			movementComponent->SetTime(0);
		}
		else
		{
			movementComponent->SetNextCellPosition(queue.front());
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(queue.front()));
		}
	}

	if (&e == player_)
	{
		elapsedTime_ = 0;
		playerCanRecoverStamina_ = false;
	}
}

void MovementSystem::SetMove(Entity& entity, const sf::Vector2i& firstCellPosition,
	const sf::Vector2i& destinationCellPosition)
{
	if (destinationCellPosition.x < 0 || destinationCellPosition.y < 0)
	{
		return;
	}

	MovementComponent* movementComponent = movementMapper.get(entity);
	std::queue<sf::Vector2i>& queue = movementComponent->GetQueue();

	if (firstCellPosition.x != -1 && firstCellPosition.y != -1 &&
		(firstCellPosition.x != destinationCellPosition.x || firstCellPosition.y != destinationCellPosition.y))
	{
		queue.push(firstCellPosition);
		if (zone_.IsMovable(destinationCellPosition))
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(destinationCellPosition));
		}
		else
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(firstCellPosition));
		}
	}
	else
	{
		queue.push(destinationCellPosition);
		movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(destinationCellPosition));
	}

	movementComponent->SetNextCellPosition(queue.front());

	AttributeComponent* attributeComponent = attributeMapper.get(entity);
	if (attributeComponent->GetState() != StateType::Walking && attributeComponent->GetState() != StateType::Running)
	{
		attributeComponent->SetState(StateType::Walking);
	}
	movementComponent->SetRunning(false);
}

void MovementSystem::SetRunMove(Entity& entity, const sf::Vector2i& firstCellPosition,
	const sf::Vector2i& destinationCellPosition, const sf::Vector2i& nextCellPosition)
{
	if (destinationCellPosition.x < 0 || destinationCellPosition.y < 0)
	{
		return;
	}

	MovementComponent* movementComponent = movementMapper.get(entity);
	std::queue<sf::Vector2i>& queue = movementComponent->GetQueue();

	if (firstCellPosition.x != -1 && firstCellPosition.y != -1 &&
		nextCellPosition.x  != -1 && nextCellPosition.y  != -1)
	{
		queue.push(firstCellPosition);
		queue.push(nextCellPosition);
		if (zone_.IsMovable(destinationCellPosition))
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(destinationCellPosition));
		}
		else
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(nextCellPosition));
		}
	}
	else if (firstCellPosition.x != -1 && firstCellPosition.y != -1 &&
			 nextCellPosition.x  == -1 && nextCellPosition.y  == -1)
	{
		queue.push(firstCellPosition);
		if (zone_.IsMovable(destinationCellPosition))
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(destinationCellPosition));
		}
		else
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(firstCellPosition));
		}
	}
	else if (nextCellPosition.x != -1 && nextCellPosition.y != -1)
	{
		queue.push(destinationCellPosition);
		queue.push(nextCellPosition);
		if (zone_.IsMovable(nextCellPosition))
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(nextCellPosition));
		}
		else
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(destinationCellPosition));
		}
	}
	else
	{
		queue.push(destinationCellPosition);
		if (zone_.IsMovable(destinationCellPosition))
		{
			movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(destinationCellPosition));
		}
	}

	movementComponent->SetNextCellPosition(queue.front());

	AttributeComponent* attributeComponent = attributeMapper.get(entity);
	if (attributeComponent->GetState() != StateType::Walking && attributeComponent->GetState() != StateType::Running)
	{
		attributeComponent->SetState(StateType::Running);
	}
	movementComponent->SetRunning(true);
}

Direction::Enum MovementSystem::GetDirection(const sf::Vector2i& a, const sf::Vector2i& b)
{
		 if (a.y <  b.y && a.x == b.x) return Direction::Down;
	else if (a.y <  b.y && a.x >  b.x) return Direction::DownLeft;
	else if (a.y == b.y && a.x >  b.x) return Direction::Left;
	else if (a.y >  b.y && a.x >  b.x) return Direction::UpLeft;
	else if (a.y >  b.y && a.x == b.x) return Direction::Up;
	else if (a.y >  b.y && a.x <  b.x) return Direction::UpRight;
	else if (a.y == b.y && a.x <  b.x) return Direction::Right;
	else if (a.y <  b.y && a.x <  b.x) return Direction::DownRight;
	else return Direction::Down;
}

void MovementSystem::EnsurePlayerEntity()
{
	if (player_ == nullptr)
	{
		player_ = &world->getTagManager()->getEntity("PLAYER");
	}
}

void MovementSystem::SendPlayerMove()
{
	if (player_ == nullptr)
	{
		return;
	}

	AttributeComponent* attributeComponent = attributeMapper.get(*player_);
	MovementComponent* movementComponent = movementMapper.get(*player_);

	if (attributeComponent->GetState() == StateType::Walking)
	{
		attributeComponent->IncreaseStamina(1);

		Network::GamePacket packet(Network::GameProtocol::PKT_MOVEMIDDLE);
		packet << static_cast<sf::Uint16>(movementComponent->GetNextCellPosition().x)
			   << static_cast<sf::Uint16>(movementComponent->GetNextCellPosition().y)
			   << static_cast<sf::Uint16>(GetDirection(
					movementComponent->GetStartCellPosition(),
					movementComponent->GetNextCellPosition()))
			   << 0u;
		Game::Instance().gameSocket->Send(packet);
	}
	else if (attributeComponent->GetState() == StateType::Running)
	{
		Network::GamePacket packet(Network::GameProtocol::PKT_RUN_MOVEMIDDLE);
		packet << static_cast<sf::Uint16>(movementComponent->GetNextCellPosition().x)
			   << static_cast<sf::Uint16>(movementComponent->GetNextCellPosition().y)
			   << static_cast<sf::Uint16>(movementComponent->GetSentNextRunCellPosition().x)
			   << static_cast<sf::Uint16>(movementComponent->GetSentNextRunCellPosition().y)
			   << static_cast<sf::Uint16>(GetDirection(
					movementComponent->GetStartCellPosition(),
					movementComponent->GetNextCellPosition()))
			   << 0u;
		Game::Instance().gameSocket->Send(packet);
	}
}

void MovementSystem::SendPlayerRunToggle(const bool runToggle)
{
	Network::GamePacket packet(Network::GameProtocol::PKT_SETRUNMODE);
		packet << runToggle;
	Game::Instance().gameSocket->Send(packet);
}

void MovementSystem::UpdatePlayerStamina()
{
	elapsedTime_ += world->getDelta();
	if (!playerCanRecoverStamina_)
	{
		playerStaminaRecoveryDelay_ = 2.6f;
	}

	if (elapsedTime_ >= playerStaminaRecoveryDelay_)
	{
		AttributeComponent* attributeComponent = attributeMapper.get(*player_);
		attributeComponent->IncreaseStamina(5);

		elapsedTime_ -= playerStaminaRecoveryDelay_;

		if (!playerCanRecoverStamina_)
		{
			playerCanRecoverStamina_ = true;
			playerStaminaRecoveryDelay_ = 1.0f;
		}
	}
}
