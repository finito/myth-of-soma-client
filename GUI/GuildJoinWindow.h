#ifndef GUI_GUILD_JOIN_WINDOW_H
#define GUI_GUILD_JOIN_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace CEGUI
{
	class CustomItemListbox;
}

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class GuildJoinWindow
{
public:
	GuildJoinWindow();
	~GuildJoinWindow();

	void HandleApplicationList(Network::GamePacket& packet);
	void HandleRequestPlayerInfo(Network::GamePacket& packet);
	void HandleApplicationReject(Network::GamePacket& packet);
	void HandleApplicationJoin(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);

	bool HandleCloseButtonClicked(const CEGUI::EventArgs& e);
	bool HandleRejectButtonClicked(const CEGUI::EventArgs& e);
	bool HandleJoinButtonClicked(const CEGUI::EventArgs& e);
	bool HandleApplcationListItemMouseEnters(const CEGUI::EventArgs& e);

	void HandleApplicationErrorCode(Network::GamePacket& packet);

private:
	CEGUI::Window*		window_;
	CEGUI::PushButton*	closeButton_;
	CEGUI::PushButton*	rejectButton_;
	CEGUI::PushButton*	joinButton_;
	CEGUI::CustomItemListbox* applicationList_;
	bool				processing_;
	CEGUI::ItemEntry*	processApplicationItem_;
};

} // namespace GUI

#endif // GUI_GUILD_JOIN_WINDOW_H