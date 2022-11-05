#ifndef GAME_H
#define GAME_H

#include <boost/scoped_ptr.hpp>
#include "Artemis/Entity.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>

namespace Network
{
	class GameSocket;
	class GamePacket;
	class SessionSocket;
	class SessionPacket;
}

namespace GUI
{
	class ItemTooltip;
	class MessageBoxWindow;
	class InputBoxWindow;
	class GuildInfoWindow;
	class GuildEmblem;
}

namespace Framework
{
	class GUI;
}

namespace artemis
{
	class Entity;
}

class ItemTable;
class MagicTable;

class Game
{
public:
	static Game& Instance()
	{
		return instance_;
	}

	void Cleanup();

	~Game();

	boost::scoped_ptr<Network::GameSocket> gameSocket;
	boost::scoped_ptr<Network::SessionSocket> sessionSocket;

	// Shared State Data
	std::string gameServerIp;
	int			gameServerPort;
	std::string	username;		 // The account username that has logged in
	std::string	password;		 // The account password that has logged in

	GUI::ItemTooltip* itemTooltip;
	GUI::MessageBoxWindow* messageBoxWindow;
	GUI::InputBoxWindow* inputBoxWindow;
	GUI::GuildInfoWindow* guildInfoWindow;
	GUI::GuildEmblem* guildEmblem;

	Framework::GUI* gui;

	artemis::Entity* myEntity;

	unsigned int zoneNumber;
	sf::Vector2i zoneCellSize;
	sf::Vector2f zonePixelSize;

	boost::scoped_ptr<ItemTable> itemTable;
	boost::scoped_ptr<MagicTable> magicTable;

	sf::Shader paletteShader;
	sf::Shader paletteGrayscaleShader;
	sf::Shader paletteOverlayShader;
	sf::Shader shadowShader;
	sf::Texture* lastPaletteTexture;

private:
	static Game instance_;
	Game();
};

#endif // GAME_H