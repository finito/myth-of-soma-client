#ifndef GUI_GUILD_APPLY_WINDOW_H
#define GUI_GUILD_APPLY_WINDOW_H

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

class GuildApplyWindow
{
public:
	GuildApplyWindow();
	~GuildApplyWindow();

	void HandleGuildList(Network::GamePacket& packet);
	void HandleRequestGuildInfo(Network::GamePacket& packet);
	void HandleApplyToGuild(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);

	bool HandleCloseButtonClicked(const CEGUI::EventArgs& e);
	bool HandleApplyButtonClicked(const CEGUI::EventArgs& e);
	bool HandleGuildListItemMouseEnters(const CEGUI::EventArgs& e);

private:
	CEGUI::Window* window_;
	CEGUI::PushButton*	closeButton_;
	CEGUI::PushButton*	applyButton_;
	CEGUI::CustomItemListbox* guildList_;
	bool				processing_;
	CEGUI::ItemEntry*	processGuildItem_;
};

} // namespace GUI

#endif // GUI_GUILD_APPLY_WINDOW_H