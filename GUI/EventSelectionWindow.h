#ifndef GUI_EVENT_SELECTION_WINDOW_H
#define GUI_EVENT_SELECTION_WINDOW_H

#include <CEGUI/CEGUI.h>
#include <list>

namespace GUI
{

class EventSelectionWindow
{
public:
	static const int SELECTION_COUNT = 4;

	EventSelectionWindow();
	~EventSelectionWindow();

	void SetMessage(const std::string& message);
	void SetSelection(const std::string& selection, int index);

private:
	void ShowSelections();
	void SendSelection(unsigned int selectionNumber);

	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);
	bool HandleMouseClick(const CEGUI::EventArgs& e);
	bool HandleSelectionMouseClick(const CEGUI::EventArgs& e);
	bool HandleSelectionMouseEnters(const CEGUI::EventArgs& e);
	bool HandleSelectionMouseLeaves(const CEGUI::EventArgs& e);

private:
	CEGUI::Window* window_;
	CEGUI::Window* text_;
	CEGUI::Window* selections_[SELECTION_COUNT];
};

} // namespace GUI

#endif // GUI_EVENT_SELECTION_WINDOW_H