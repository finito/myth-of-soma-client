
#include "Game.h"
#include "Network/GameSocket.h"
#include "Network/SessionSocket.h"
#include "GUI/ItemTooltip.h"
#include "GUI/MessageBoxWindow.h"
#include "GUI/InputBoxWindow.h"
#include "GUI/GuildInfoWindow.h"
#include "GUI/GuildEmblem.h"
#include "Framework/GUI.h"
#include "DataTables/ItemTable.h"
#include "DataTables/MagicTable.h"

Game Game::instance_;

Game::Game() :
gui(nullptr),
itemTooltip(nullptr),
messageBoxWindow(nullptr),
inputBoxWindow(nullptr),
guildInfoWindow(nullptr),
guildEmblem(nullptr),
itemTable(new ItemTable),
magicTable(new MagicTable)
{
	paletteShader.loadFromFile("shaders/palette.frag", sf::Shader::Fragment);
	paletteGrayscaleShader.loadFromFile("shaders/paletteGrayscale.frag", sf::Shader::Fragment);
	paletteOverlayShader.loadFromFile("shaders/paletteOverlay.frag", sf::Shader::Fragment);
	shadowShader.loadFromFile("shaders/shadow.frag", sf::Shader::Fragment);
	lastPaletteTexture = nullptr;

	itemTable->Load("itemtable.xml");
	magicTable->Load("magictable.xml");
}

Game::~Game()
{
}

void Game::Cleanup()
{
	myEntity = nullptr;

	gui = nullptr;

	itemTooltip = nullptr;	
	messageBoxWindow = nullptr;
	inputBoxWindow = nullptr;
	guildInfoWindow = nullptr;
	guildEmblem = nullptr;
}