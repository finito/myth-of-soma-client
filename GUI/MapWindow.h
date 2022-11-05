#ifndef GUI_MAP_WINDOW_H
#define GUI_MAP_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

class MapWindow
{
public:
	MapWindow();
	~MapWindow();

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleWindowUpdated(const CEGUI::EventArgs& e);

	bool SetImage();

private:
	CEGUI::Window*	window_;
	CEGUI::Window*  playerPosition_;

	unsigned int zoneNumber_;
};

} // namespace GUI

#endif // GUI_MAP_WINDOW_H