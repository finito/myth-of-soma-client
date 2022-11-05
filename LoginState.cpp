
#include "LoginState.h"
#include "SelectCharacterState.h"

#include "GUI/LoginWindow.h"

#include "Network/SessionSocket.h"
#include "Network/SessionPacket.h"
#include "Network/SessionProtocol.h"

#include "Game.h"

#include <SFML/Network/IpAddress.hpp>
#include <boost/make_shared.hpp>

LoginState::LoginState(Framework::GameEngine& engine) :
engine_(engine),
loginWindow_(new GUI::LoginWindow)
{
	loginWindow_->SetOKButtonEventHandler(CEGUI::Event::Subscriber(&LoginState::HandleLogin, this));
	loginWindow_->SetCancelButtonEventHandler(CEGUI::Event::Subscriber(&LoginState::HandleQuit, this));

	// TODO : Look into a way to better the handling of the session socket between multiple game states
	// The session socket is shared between multiple states.
	if (!Game::Instance().sessionSocket)
	{
		Game::Instance().sessionSocket.reset(new Network::SessionSocket);
	}
}

LoginState::~LoginState()
{
}

void LoginState::DoUpdate(const float deltaTime)
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

void LoginState::DoRender()
{
	engine_.gui.GetSystem().renderGUI();
}

bool LoginState::HandleLogin(const CEGUI::EventArgs& e)
{
	const std::string username = loginWindow_->GetUsernameText();
	const std::string password = loginWindow_->GetPasswordText();
	if (username.empty() || password.empty())
	{
		loginWindow_->ShowMessageText("Be sure to have both your username and password filled in.");
	}
	else
	{
		Game::Instance().sessionSocket->disconnect();
		Game::Instance().sessionSocket->setBlocking(true);
		loginWindow_->ShowMessageText("Connecting to the server...\nThis may take some time...");
#ifdef CONNECT_TO_TEST_SERVER
		const std::string ip = "somastaging";
#else
		const std::string ip = "192.168.2.15";
#endif
		if (Game::Instance().sessionSocket->connect(sf::IpAddress(ip), 4110, sf::seconds(10.f)) != Network::SessionSocket::Done)
		{
			loginWindow_->ShowMessageText("Connection to the server failed. Please try again later.");
		}
		else
		{
			Game::Instance().sessionSocket->setBlocking(false);
			Network::SessionPacket packet(Network::SessionProtocol::SM_GAMEINFO2_REQ);
			Game::Instance().sessionSocket->Send(packet);
		}
	}
	return true;
}

bool LoginState::HandleQuit(const CEGUI::EventArgs& e)
{
	engine_.Quit();
	return true;
}

bool LoginState::HandleAccountButton(const CEGUI::EventArgs& e)
{
	return true;
}

bool LoginState::HandleRegisterButton(const CEGUI::EventArgs& e)
{
	return true;
}

void LoginState::HandleNetwork()
{
	// Attempt to receieve a packet
	Network::SessionPacket packet;
	if (Game::Instance().sessionSocket->Receive(packet) == Network::SessionSocket::Done)
	{
		// Do something if receieved a packet
		sf::Uint16 packetId = 0;
		packet >> packetId;

		switch (packetId)
		{
		case Network::SessionProtocol::SM_GAMEINFO_ACK:
			HandleGameInfoAck(packet);
			break;
		case Network::SessionProtocol::SM_LOGIN_ACK:
			HandleLoginAck(packet);
			break;
		case Network::SessionProtocol::SM_GAME_ACK:
			HandleGameAck(packet);
			break;
		case Network::SessionProtocol::SM_CONNECTINFO_ACK:
			HandleConnectInfoAck(packet);
			break;
		case Network::SessionProtocol::SM_UPGRADING_ACK:
			HandleUpgradingAck(packet);
			// Server Notice
			break;
		}
	}
}

void LoginState::HandleGameInfoAck(Network::SessionPacket& packetRecv)
{
	std::string sessionString = "soma";
	Network::SessionPacket packetSend(Network::SessionProtocol::SM_LOGIN_REQ);
	packetSend << loginWindow_->GetUsernameText()
			   << loginWindow_->GetPasswordText()
			   << sessionString
			   << sessionString;
	Game::Instance().sessionSocket->Send(packetSend);
}

void LoginState::HandleLoginAck(Network::SessionPacket& packetRecv)
{
	sf::Uint8 result = -1;
	packetRecv >> result;

	switch (result)
	{
	case 0:
	case 1:
		{
		Network::SessionPacket packetSend(Network::SessionProtocol::SM_GAME_REQ);
		packetSend << static_cast<sf::Int32>(0);
		packetSend << static_cast<sf::Uint8>(1);
		Game::Instance().sessionSocket->Send(packetSend);
		loginWindow_->ShowMessageText( "You logged-in for free.\nPress enter to continue.");
		}
		break;
	case 2:
		loginWindow_->ShowMessageText("Your free game trial has run out.");
		break;
	case 3:
		loginWindow_->ShowMessageText("Please enter a password.");
		break;
	// TODO: Need to merge these into one on the session service!!!
	case 6:
	case 7:
		loginWindow_->ShowMessageText("Username and/or Password is incorrect.\nPress enter to try again.");
		break;
	default:
		loginWindow_->ShowMessageText("Login failed. Please try again later.");
		break;
	}
}

void LoginState::HandleGameAck(Network::SessionPacket& packetRecv)
{
	 // I think this is where need to change to select character state
	// and then begin by sending account login information.
	// FIND OUT: Think this will not be used!
}

void LoginState::HandleConnectInfoAck(Network::SessionPacket& packetRecv)
{
	std::string username = "";
	packetRecv >> username;

	const std::string password = loginWindow_->GetPasswordText();

	bool start = false;
	packetRecv >> start;

	std::string gameServerIp = "";
	packetRecv >> gameServerIp;

	sf::Int32 gameServerPort = 0;
	packetRecv >> gameServerPort;

	Game::Instance().gameServerIp = gameServerIp;
	Game::Instance().gameServerPort = gameServerPort;
	Game::Instance().username = username;
	Game::Instance().password = password;

	engine_.ChangeState(boost::make_shared<SelectCharacterState>(engine_));
}

void LoginState::HandleUpgradingAck(Network::SessionPacket& packetRecv)
{
}