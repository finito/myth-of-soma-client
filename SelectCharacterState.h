#ifndef SELECT_CHARACTER_STATE_H
#define SELECT_CHARACTER_STATE_H

#include "Framework/GameEngine.h"
#include <boost/scoped_ptr.hpp>

namespace GUI
{
	class SelectCharacterWindow;
}

namespace Network
{
	class GamePacket;
}

class SelectCharacterState : public Framework::IGameState
{
public:
	SelectCharacterState(Framework::GameEngine& engine);
	~SelectCharacterState();

protected:
	virtual void DoUpdate(float deltaTime);
	virtual void DoRender();

private:
	bool HandleQuit(const CEGUI::EventArgs& e);
	bool HandleStartGame(const CEGUI::EventArgs& e);

	void HandleNetwork();
	void SendAccountLogin();
	void HandleAccountLogin(Network::GamePacket& packetRecv);

private:
	Framework::GameEngine& engine_;
	boost::scoped_ptr<GUI::SelectCharacterWindow> selectCharacterWindow_;
};

#endif // SELECT_CHARACTER_STATE_H