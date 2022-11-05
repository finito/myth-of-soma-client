#ifndef GUI_GUILD_INFO_WINDOW_H
#define GUI_GUILD_INFO_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

namespace GuildInfoResult
{
	enum Enum
	{
		OK,
		Cancel,
	};
}

class GuildInfoWindow
{
public:
	static CEGUI::String EventGuildInfoResult;

	GuildInfoWindow();
	~GuildInfoWindow();

	void Show(const CEGUI::String& name, const CEGUI::String& text, const bool nameReadOnly, CEGUI::Event::Subscriber resultHandler);

	GuildInfoResult::Enum GetResult() const;

	const CEGUI::String GetName() const;
	void SetName(const CEGUI::String& value);

	const CEGUI::String GetText() const;
	void SetText(const CEGUI::String& value);

private:
	void Hide();

	bool HandleOKButtonClicked(const CEGUI::EventArgs& e);
	bool HandleCancelButtonClicked(const CEGUI::EventArgs& e);

	bool HandleKeyPress(const CEGUI::EventArgs& e);

private:
	CEGUI::Window* window_;
	CEGUI::PushButton* okButton_;
	CEGUI::PushButton* cancelButton_;
	CEGUI::Editbox* name_;
	CEGUI::MultiLineEditbox* text_;
	GuildInfoResult::Enum result_;
};

} // namespace GUI

#endif // GUI_GUILD_INFO_WINDOW_H