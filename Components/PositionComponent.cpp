
#include "Components/PositionComponent.h"
#include "Framework/Utils/PositionConvert.h"

using namespace artemis;

PositionComponent::PositionComponent(const sf::Vector2f& position)
{
	SetPosition(position);
}

PositionComponent::PositionComponent(const sf::Vector2i& cellPosition)
{
	SetCellPosition(cellPosition);
}

sf::Vector2f PositionComponent::GetPosition()
{
	return position_;
}

void PositionComponent::SetPosition(const sf::Vector2f& value)
{
	position_ = value;
	cellPosition_ = Utils::PositionConvert::PixelToCell(position_);
}

sf::Vector2i PositionComponent::GetCellPosition()
{
	return cellPosition_;
}

void PositionComponent::SetCellPosition(const sf::Vector2i& value)
{
	cellPosition_ = value;
	position_ = Utils::PositionConvert::CellToPixel(cellPosition_);
}