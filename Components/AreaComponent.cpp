#include "Components/AreaComponent.h"

using namespace artemis;

sf::IntRect AreaComponent::GetArea()
{
	return area_;
}

void AreaComponent::SetArea(sf::IntRect value)
{
	area_ = value;
}

bool AreaComponent::IsTransparentPixel(const sf::Vector2i& position) const
{
	const int x = position.x - area_.left;
	const int y = position.y - area_.top;
	if (x < 0 || x >= area_.width || y < 0 || y >= area_.height)
	{
		return false;
	}
	return transparentPixels_->at(y*area_.width+x);
}

void AreaComponent::SetTransparentPixels(const std::vector<bool>* value)
{
	transparentPixels_ = value;
}