#ifndef ZONE_H
#define ZONE_H

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <map>
#include "Artemis/Entity.h"

class TileMap;
class MapObjectLoader;
class TileProperties;
class MapLight;

namespace Framework
{
	class Light;
}

// A zone is a single map within the game.
// Providing a convenient way to load, render.
class Zone
{
public:
	Zone();
	~Zone();

	void Load(unsigned int zone);

	bool IsMovable(const sf::Vector2i& cell) const;
	int  GetEvent (const sf::Vector2i& cell) const;
	int  GetHouse (const sf::Vector2i& cell) const;
	int  GetArea  (const sf::Vector2i& cell) const;
	void SetMovable(const sf::Vector2i& cell, bool value);

	Framework::Light* AddLight(const sf::Vector2f& position, const sf::Vector2f& scale,
							const sf::Color& color);

	void AddServerEntity(unsigned int serverId, artemis::Entity& entity);
	artemis::Entity* GetServerEntity(unsigned int serverId) const;
	void RemoveServerEntity(unsigned int serverId);

	void AddItemEntity(const sf::Vector2i& cellPosition, artemis::Entity& entity);
	artemis::Entity* GetItemEntity(const sf::Vector2i& cellPosition) const;
	void RemoveItemEntity(const sf::Vector2i& cellPosition);

	TileMap* GetTileMap() const;
	MapObjectLoader* GetMapObjectLoader() const;
	MapLight* GetMapLight() const;

	std::map<unsigned int, artemis::Entity*>& GetServerEntityMap();

private:
	boost::scoped_ptr<TileMap>		   tileMap_;
	boost::scoped_ptr<MapObjectLoader> mapObjectLoader_;
	boost::scoped_ptr<TileProperties>  tileProperties_;
	boost::scoped_ptr<MapLight>		   mapLight_;

	std::map<int, artemis::Entity*> itemEntityMap_;

	std::map<unsigned int, artemis::Entity*> serverEntityMap_; // A map of entities with a server id
};

#endif // ZONE_H