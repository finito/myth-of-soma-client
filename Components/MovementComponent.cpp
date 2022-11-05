
#include "Components/MovementComponent.h"

using namespace artemis;

MovementComponent::MovementComponent() :
startCellPosition_(sf::Vector2i(-1, -1)),
nextCellPosition_(sf::Vector2i(-1, -1)),
sentNextRunCellPosition_(sf::Vector2i(-1, -1)),
time_(0)
{
}

std::queue<sf::Vector2i>& MovementComponent::GetQueue()
{
	return queue_;
}

sf::Vector2i MovementComponent::GetStartCellPosition() const
{
	return startCellPosition_;
}

void MovementComponent::SetStartCellPosition(const sf::Vector2i& value)
{
	startCellPosition_ = value;
}

sf::Vector2i MovementComponent::GetNextCellPosition() const
{
	return nextCellPosition_;
}

void MovementComponent::SetNextCellPosition(const sf::Vector2i& value)
{
	nextCellPosition_ = value;
}

sf::Vector2f MovementComponent::GetDestinationPosition() const
{
	return destinationPosition_;
}

void MovementComponent::SetDestinationPosition(const sf::Vector2f& value)
{
	destinationPosition_ = value;
}

float MovementComponent::GetTime()
{
	return time_;
}

void MovementComponent::SetTime(const float value)
{
	time_ = value;
}

void MovementComponent::IncreaseTime(const float value)
{
	time_ += value;
}

void MovementComponent::DecreaseTime(const float value)
{
	time_ -= value;
}

bool MovementComponent::GetRunning() const
{
	return running_;
}

void MovementComponent::SetRunning(bool value)
{
	running_ = value;
}

sf::Vector2i MovementComponent::GetSentNextRunCellPosition() const
{
	return sentNextRunCellPosition_;
}

void MovementComponent::SetSentNextRunCellPosition(const sf::Vector2i& value)
{
	sentNextRunCellPosition_ = value;
}