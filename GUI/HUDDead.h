#ifndef GUI_HUD_DEAD_H
#define GUI_HUD_DEAD_H

#include <CEGUI/CEGUI.h>
#include <SFML/System/Clock.hpp>

namespace GUI
{

class HUDDead
{
public:
	HUDDead();
	~HUDDead();

	void Show();
	void Hide();

private:
	bool HandleWindowUpdated(const CEGUI::EventArgs& e);

	bool HandleReviveButtonClicked(const CEGUI::EventArgs& e);
	bool HandleWarpTownButtonClicked(const CEGUI::EventArgs& e);

	void SendReviveHere();
	void SendReviveAtTown();

private:
	CEGUI::Window*	window_;
	CEGUI::PushButton* reviveButton_;
	CEGUI::PushButton* warpTownButton_;
	CEGUI::Window* reiveCount_;

	float elapsedTime_;
	unsigned int countDown_;
};

} // namespace GUI

#endif // GUI_HUD_DEAD_H