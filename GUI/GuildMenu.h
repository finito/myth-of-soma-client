#ifndef GUI_GUILD_MENU_H
#define GUI_GUILD_MENU_H

#include <CEGUI/CEGUI.h>
#include <SFML/System/Vector2.hpp>

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class GuildMenu
{
public:
	GuildMenu();
	~GuildMenu();

	void HandleGuildOpen(Network::GamePacket& packet, sf::Vector2i position);
	void HandleCancelApplyGuild(Network::GamePacket& packet);
	void HandleMakeGuild(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);

	bool HandleApplicationButtonClicked(const CEGUI::EventArgs& e);
	bool HandleGuildButtonClicked(const CEGUI::EventArgs& e);
	bool HandleMakeGuildButtonClicked(const CEGUI::EventArgs& e);
	bool HandleApplyToGuildButtonClicked(const CEGUI::EventArgs& e);
	bool HandleCancelApplyButtonClicked(const CEGUI::EventArgs& e);
	
	bool HandleCancelApplyMessageBoxResult(const CEGUI::EventArgs& e);

	bool HandleMouseLeavesArea(const CEGUI::EventArgs& e);

	bool HandleMakeGuildMessageBoxResult(const CEGUI::EventArgs& e);

	bool HandleGuildInfoResult(const CEGUI::EventArgs& e);

	bool IsGuildNameValid(const std::string& name);

private:
	CEGUI::Window*	   window_;
	CEGUI::PushButton* applicationButton_;
	CEGUI::PushButton* guildButton_;
	CEGUI::PushButton* makeGuildButton_;
	CEGUI::PushButton* applyToGuildButton_;
	CEGUI::PushButton* cancelApplyButton_;
	CEGUI::PushButton* makeGuildOKButton_;
	CEGUI::PushButton* makeGuildCancelButton_;
	CEGUI::SequentialLayoutContainer* layout_;

	unsigned int guildMoney_;
	short guildLevel_;
	std::string guildName_;
	bool applied_;
	bool processing_;
};

} // namespace GUI

#endif // GUI_GUILD_MENU_H