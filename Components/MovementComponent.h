#ifndef COMPONENTS_MOVEMENT_COMPONENT_H
#define COMPONENTS_MOVEMENT_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <SFML/System/Vector2.hpp>
#include <queue>

class MovementComponent : public artemis::Component
{
public:
	MovementComponent();

	std::queue<sf::Vector2i>& GetQueue();

	sf::Vector2i GetStartCellPosition() const;
	void SetStartCellPosition(const sf::Vector2i& value);

	sf::Vector2i GetNextCellPosition() const;
	void SetNextCellPosition(const sf::Vector2i& value);

	sf::Vector2f GetDestinationPosition() const;
	void SetDestinationPosition(const sf::Vector2f& value);

	float GetTime();
	void SetTime(float value);
	void IncreaseTime(float value);
	void DecreaseTime(float value);

	bool GetRunning() const;
	void SetRunning(bool value);

	sf::Vector2i GetSentNextRunCellPosition() const;
	void SetSentNextRunCellPosition(const sf::Vector2i& value);

private:
	std::queue<sf::Vector2i> queue_;
	sf::Vector2i startCellPosition_;
	sf::Vector2i nextCellPosition_;
	sf::Vector2i sentNextRunCellPosition_;
	sf::Vector2f destinationPosition_;
	float time_;
	bool running_;
};

#endif // COMPONENTS_MOVEMENT_COMPONENT_H