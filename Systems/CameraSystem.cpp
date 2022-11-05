
#include "Systems/CameraSystem.h"
#include "Components/PositionComponent.h"
#include <cmath>
#include "Game.h"

using namespace artemis;

CameraSystem::CameraSystem(sf::RenderTarget& renderTarget) :
renderTarget_(renderTarget),
player_(nullptr)
{

}

void CameraSystem::initialize()
{
	camera_.setSize(static_cast<sf::Vector2f>(renderTarget_.getSize()));

	positionMapper.init(*world);
}

void CameraSystem::processEntities(ImmutableBag<Entity*>& entities)
{
	EnsurePlayerEntity();
	if (player_ != nullptr)
	{
		UpdateCenter();
	}
}

bool CameraSystem::checkProcessing()
{
	return true;
}

void CameraSystem::EnsurePlayerEntity()
{
	if (player_ == nullptr)
	{
		player_ = &world->getTagManager()->getEntity("PLAYER");
	}
	else if (!player_->isActive())
	{
		player_ = nullptr;
	}
}

float CameraSystem::GetWidth() const
{
	return camera_.getSize().x;
}

float CameraSystem::GetHeight() const
{
	return camera_.getSize().y;
}

sf::Vector2f CameraSystem::GetSize() const
{
	return camera_.getSize();
}

float CameraSystem::GetX() const
{
	return std::ceil(camera_.getCenter().x) - (GetWidth() / 2);
}

float CameraSystem::GetY() const
{
	return std::ceil(camera_.getCenter().y) - (GetHeight() / 2);
}

sf::Vector2f CameraSystem::GetCenter() const
{
	return sf::Vector2f(std::ceil(camera_.getCenter().x), std::ceil(camera_.getCenter().y));
}

sf::Vector2f CameraSystem::GetOffset() const
{
	return sf::Vector2f(GetX(), GetY());
}

void CameraSystem::UpdateCenter()
{
	sf::Vector2f center = positionMapper.get(*player_)->GetPosition();
	center.x = std::max(GetWidth() / 2, center.x);
	center.y = std::max(GetHeight() / 2, center.y);

	int offsetX = (static_cast<int> (GetWidth() / 2) % 32) > 0 ? 32 : 0;
	int offsetY = (static_cast<int> (GetHeight() / 2) % 32) > 0 ? 32 : 0;
	center.x = std::min(Game::Instance().zonePixelSize.x - (GetWidth() / 2) - 32 - offsetX, center.x);
	center.y = std::min(Game::Instance().zonePixelSize.y - (GetHeight() / 2) - 32 - offsetY, center.y);

	camera_.setCenter(sf::Vector2f(std::ceil(center.x), std::ceil(center.y)));
}

const sf::View& CameraSystem::GetCamera() const
{
	return camera_;
}