
#include "Zone.h"
#include "TileMap.h"
#include "MapObjectLoader.h"
#include "TileProperties.h"
#include "MapLight.h"
#include "Game.h"

#include "Components/PositionComponent.h"

#include "Framework/Graphics/Light.h"

#include <boost/format.hpp>

Zone::Zone() :
tileMap_(new TileMap),
mapObjectLoader_(new MapObjectLoader(*this)),
tileProperties_(new TileProperties),
mapLight_(new MapLight)
{
}

Zone::~Zone()
{
}

void Zone::Load(const unsigned int zone)
{
	const std::string zonename = boost::str(boost::format("map/mset%02i") % zone);
	tileMap_->Load(zonename);
	mapObjectLoader_->Load(zonename + ".mod");
	tileProperties_->Load(zonename + ".imf");
	mapLight_->Load(zonename + ".lsd");

	Game::Instance().zoneNumber = zone;
	Game::Instance().zoneCellSize = sf::Vector2i(tileProperties_->GetWidth(), tileProperties_->GetHeight());
	Game::Instance().zonePixelSize = sf::Vector2f(tileMap_->GetWidth() * 32.0f, tileMap_->GetHeight() * 32.0f);
}

bool Zone::IsMovable(const sf::Vector2i& cell) const
{
	return tileProperties_->GetMovable(cell);
}

int Zone::GetEvent(const sf::Vector2i& cell) const
{
	return tileProperties_->GetEvent(cell);
}

int Zone::GetHouse(const sf::Vector2i& cell) const
{
	return tileProperties_->GetHouse(cell);
}

int Zone::GetArea(const sf::Vector2i& cell) const
{
	return tileProperties_->GetArea(cell);
}

void Zone::SetMovable(const sf::Vector2i& cell, bool value)
{
	tileProperties_->SetMovable(cell, value);
}

Framework::Light* Zone::AddLight(const sf::Vector2f& position, const sf::Vector2f& scale, const sf::Color& color)
{
	return mapLight_->AddLight(position, scale, color);
}

void Zone::AddServerEntity(const unsigned int serverId, artemis::Entity& entity)
{
	if (GetServerEntity(serverId) == nullptr)
	{
		serverEntityMap_[serverId] = &entity;
	}
}

artemis::Entity* Zone::GetServerEntity(const unsigned int serverId) const
{
	auto it = serverEntityMap_.find(serverId);
	return it != serverEntityMap_.cend() ? it->second : nullptr;
}

void Zone::RemoveServerEntity(const unsigned int serverId)
{
	serverEntityMap_.erase(serverId);
}

void Zone::AddItemEntity(const sf::Vector2i& position, artemis::Entity& entity)
{
	itemEntityMap_[position.x << 16 | position.y] = &entity;
}

artemis::Entity* Zone::GetItemEntity(const sf::Vector2i& position) const
{
	auto it = itemEntityMap_.find(position.x << 16 | position.y);
	return it != itemEntityMap_.cend() ? it->second : nullptr;
}

void Zone::RemoveItemEntity(const sf::Vector2i& position)
{
	auto it = itemEntityMap_.find(position.x << 16 | position.y);
	if (it != itemEntityMap_.cend())
	{
		itemEntityMap_.erase(position.x << 16 | position.y);
	}
}

TileMap* Zone::GetTileMap() const
{
	return tileMap_.get();
}

MapObjectLoader* Zone::GetMapObjectLoader() const
{
	return mapObjectLoader_.get();
}

MapLight* Zone::GetMapLight() const
{
	return mapLight_.get();
}

std::map<unsigned int, artemis::Entity*>& Zone::GetServerEntityMap()
{
	return serverEntityMap_;
}