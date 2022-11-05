
#include "SelectCharacterState.h"
#include "PlayState.h"

#include "GUI/SelectCharacterWindow.h"

#include "Network/GameSocket.h"
#include "Network/SessionSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include "Game.h"

#include <SFML/Network/IpAddress.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>

SelectCharacterState::SelectCharacterState(Framework::GameEngine& engine) :
engine_(engine),
selectCharacterWindow_(new GUI::SelectCharacterWindow)
{
	selectCharacterWindow_->SetExitButtonEventHandler(CEGUI::Event::Subscriber(&SelectCharacterState::HandleQuit, this));
	selectCharacterWindow_->SetOKButtonEventHandler(CEGUI::Event::Subscriber(&SelectCharacterState::HandleStartGame, this));

	// TODO : Look into a way to better the handling of the game socket between multiple game states

	// Only connect to the game server if the socket is not already initialized and connected.
	// The game socket is shared between multiple states.
	if (!Game::Instance().gameSocket)
	{
		Game::Instance().gameSocket.reset(new Network::GameSocket);

		// Attempt a connection to the game server
		if (Game::Instance().gameSocket->connect(sf::IpAddress(Game::Instance().gameServerIp),
			Game::Instance().gameServerPort, sf::seconds(10.f)) != Network::GameSocket::Done)
		{

			//_selectCharacterWindow->ShowMessageText("Connection to the server failed. Please try again later.");
		}
		else
		{
			Game::Instance().gameSocket->setBlocking(false);
			Network::GamePacket packet(Network::GameProtocol::PKT_ENCRYPTION_START_REQ, false);
#ifdef CONNECT_TO_TEST_SERVER
			packet << static_cast<sf::Uint8>(0x1D)
				<< static_cast<sf::Uint8>(0x00)
				<< static_cast<sf::Uint16>(0x0000)
				<< static_cast<sf::Uint8>(0x01);
#else
			packet << static_cast<sf::Uint8>(0xd1)
				<< static_cast<sf::Uint8>(0x07)
				<< static_cast<sf::Uint16>(0x0000)
				<< static_cast<sf::Uint8>(0x01);
#endif
			Game::Instance().gameSocket->Send(packet);
		}
	}
	else
	{
		// Already connected so can continue by sending account login.
		SendAccountLogin();
	}
}

SelectCharacterState::~SelectCharacterState()
{
}

void SelectCharacterState::DoUpdate(const float deltaTime)
{
    sf::Event e;
    while (engine_.renderWindow.pollEvent(e))
    {
		if (!engine_.gui.HandleEvent(engine_.renderWindow, e))
		{
			if (e.type == sf::Event::Closed)
			{
				engine_.Quit();
			}
		}
    }
	engine_.gui.GetSystem().injectTimePulse(deltaTime);

	HandleNetwork();
	Game::Instance().sessionSocket->SendKeepAlive();
}

void SelectCharacterState::DoRender()
{
	engine_.gui.GetSystem().renderGUI();
}

bool SelectCharacterState::HandleQuit(const CEGUI::EventArgs& e)
{
	engine_.Quit();
	return true;
}

bool SelectCharacterState::HandleStartGame(const CEGUI::EventArgs& e)
{
	const std::string characterName = selectCharacterWindow_->GetActiveCharacterName();
	if (!characterName.empty())
	{
		engine_.ChangeState(boost::make_shared<PlayState>(engine_, characterName));
	}
	else
	{
		// TODO: Log and/or show a message
	}
	return true;
}

void SelectCharacterState::HandleNetwork()
{
	// Attempt to receive a packet
	Network::GamePacket packet;
	if (Game::Instance().gameSocket->Receive(packet) == Network::GameSocket::Done)
	{
		// Do something if received a packet
		sf::Uint8 packetId = 0;
		packet >> packetId;

		switch (packetId)
		{
		case Network::GameProtocol::PKT_ENCRYPTION_START_REQ:
			if (Game::Instance().gameSocket->HandleEncryptionStartResult(packet))
			{
				SendAccountLogin();
			}
			else
			{
				// TODO: Disconnection message / version error
			}
		break;
		case Network::GameProtocol::PKT_ACCOUNT_LOGIN:
			HandleAccountLogin(packet);
			break;
		}
	}
}

void SelectCharacterState::SendAccountLogin()
{
	Network::GamePacket packet(Network::GameProtocol::PKT_ACCOUNT_LOGIN);
	packet << Game::Instance().username
		   << Game::Instance().password;
	Game::Instance().gameSocket->Send(packet);
}

void SelectCharacterState::HandleAccountLogin(Network::GamePacket& packetRecv)
{
	sf::Uint8 result = Network::GameProtocol::FAIL;
	packetRecv >> result;
	
	if (result == Network::GameProtocol::SUCCESS)
	{
		sf::Uint8 lastCharacterSlot = 0;
		packetRecv >> lastCharacterSlot;
		selectCharacterWindow_->SetActiveCharacterSlot(lastCharacterSlot);

		sf::Uint8 characterCount = 0;
		packetRecv >> characterCount;

		for (int i = 0; i < characterCount; ++i)
		{
			GUI::SelectCharacterWindow::CharacterData characterData;
			packetRecv >> characterData.name_
					   >> characterData.class_
					   >> characterData.level_
					   >> characterData.strength_
					   >> characterData.dexterity_
					   >> characterData.intelligence_
					   >> characterData.charisma_
					   >> characterData.wisdom_
					   >> characterData.constitution_
					   >> characterData.health_
					   >> characterData.maximumHealth_
					   >> characterData.mana_
					   >> characterData.maximumMana_
					   >> characterData.stamina_
					   >> characterData.maximumStamina_
					   >> characterData.age_
					   >> characterData.moral_
					   >> characterData.gender_
					   >> characterData.hair_
					   >> characterData.hairMode_
					   >> characterData.skin_
					   >> characterData.guildName_
					   >> characterData.fame_
					   >> characterData.swordExp_
					   >> characterData.spearExp_
					   >> characterData.bowExp_
					   >> characterData.axeExp_
					   >> characterData.knuckleExp_
					   >> characterData.staffExp_
					   >> characterData.weaponMakeExp_
					   >> characterData.armorMakeExp_
					   >> characterData._accessoryMakeExp
					   >> characterData.potionMakeExp_
					   >> characterData.cookingExp_;
			for (int j = 0; j < Network::GameProtocol::EQUIP_ITEM_NUM; ++j)
			{
				packetRecv >> characterData.equippedItem_[j];
				if (characterData.equippedItem_[j] == -1)
				{
					characterData.equippedItem_[j] = 0;
				}
			}
			sf::Uint8 notUsed;
			packetRecv >> notUsed;

			boost::to_upper(characterData.name_);
			boost::to_upper(characterData.guildName_);
			characterData.gender_ = !characterData.gender_;
			selectCharacterWindow_->SetCharacterData(characterData, i);
		}

		selectCharacterWindow_->HandleCharacterDataLoadComplete();
	}
	else
	{
		// TODO: Show error message and then quit after closing it
	}
}