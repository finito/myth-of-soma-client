#ifndef GUI_GUILD_EMBLEM_CHANGE_WINDOW_H
#define GUI_GUILD_EMBLEM_CHANGE_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

namespace GuildEmblemChangeResult
{
	enum Enum
	{
		OK,
		Cancel,
	};
}

class GuildEmblemChangeWindow
{
public:
	static CEGUI::String EventGuildEmblemChangeResult;

	GuildEmblemChangeWindow();
	~GuildEmblemChangeWindow();

	bool Show(const unsigned int cost, CEGUI::Event::Subscriber resultHandler);

	GuildEmblemChangeResult::Enum GetResult() const;

private:
	void Hide();

	bool HandleOKButtonClicked(const CEGUI::EventArgs& e);
	bool HandleCancelButtonClicked(const CEGUI::EventArgs& e);

	bool HandleKeyPress(const CEGUI::EventArgs& e);

private:
	CEGUI::Window* window_;
	CEGUI::Window* guildEmblem_;
	CEGUI::Window* message_;
	CEGUI::PushButton* okButton_;
	CEGUI::PushButton* cancelButton_;
	GuildEmblemChangeResult::Enum result_;
};

} // namespace GUI

#endif // GUI_GUILD_EMBLEM_CHANGE_WINDOW_H