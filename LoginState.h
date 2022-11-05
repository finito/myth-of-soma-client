#ifndef LOGIN_STATE_H
#define LOGIN_STATE_H

#include "Framework/GameEngine.h"
#include <boost/scoped_ptr.hpp>

namespace GUI
{
	class LoginWindow;
}

namespace Network
{
	class SessionPacket;
}

class LoginState : public Framework::IGameState
{
public:
	LoginState(Framework::GameEngine& engine);
	~LoginState();

protected:
	virtual void DoUpdate(float deltaTime);
	virtual void DoRender();

private:
	bool HandleLogin(const CEGUI::EventArgs& e);
	bool HandleQuit(const CEGUI::EventArgs& e);
	bool HandleAccountButton(const CEGUI::EventArgs& e);
	bool HandleRegisterButton(const CEGUI::EventArgs& e);

	void HandleNetwork();
	void HandleGameInfoAck(Network::SessionPacket& packetRecv);
	void HandleLoginAck(Network::SessionPacket& packetRecv);
	void HandleGameAck(Network::SessionPacket& packetRecv);
	void HandleConnectInfoAck(Network::SessionPacket& packetRecv);
	void HandleUpgradingAck(Network::SessionPacket& packetRecv);

private:
	Framework::GameEngine& engine_;
	boost::scoped_ptr<GUI::LoginWindow> loginWindow_;
};

#endif // LOGIN_STATE_H