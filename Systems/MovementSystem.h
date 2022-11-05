#ifndef SYSTEMS_MOVEMENT_SYSTEM_H
#define SYSTEMS_MOVEMENT_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

#include <SFML/System/Vector2.hpp>

#include "Components/AttributeComponent.h" // For Direction Enum

class MovementComponent;
class PositionComponent;

class Zone;

class MovementSystem : public artemis::EntityProcessingSystem
{
public:
	MovementSystem(Zone& zone);

	virtual void initialize();

	virtual void processEntity(artemis::Entity& e);

	void SetMove(artemis::Entity& entity, const sf::Vector2i& firstCellPosition,
		const sf::Vector2i& destinationCellPosition);

	void SetRunMove(artemis::Entity& entity, const sf::Vector2i& firstCellPosition,
		const sf::Vector2i& destinationCellPosition, const sf::Vector2i& nextCellPosition);

	static Direction::Enum GetDirection(const sf::Vector2i& a, const sf::Vector2i& b);

	static void SendPlayerRunToggle(bool runToggle);

private:
	void EnsurePlayerEntity();
	void SendPlayerMove();
	void UpdatePlayerStamina();

	Zone& zone_;
	artemis::Entity* player_;
	float elapsedTime_;
	bool playerCanRecoverStamina_;
	float playerStaminaRecoveryDelay_;

	artemis::ComponentMapper<MovementComponent> movementMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
};

#endif // SYSTEMS_MOVEMENT_SYSTEM_H