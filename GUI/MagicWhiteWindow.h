#ifndef GUI_MAGIC_WHITE_WINDOW_H
#define GUI_MAGIC_WHITE_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

const int MAGIC_WHITE_SLOT_COUNT = 5;

class MagicWhiteWindow
{
public:
	MagicWhiteWindow();
	~MagicWhiteWindow();

private:
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleScroll(const CEGUI::EventArgs& e);
	bool HandleAuraButton(const CEGUI::EventArgs& e);
	bool HandleBlueButton(const CEGUI::EventArgs& e);
	bool HandleBlackButton(const CEGUI::EventArgs& e);
	bool HandleAbilityButton(const CEGUI::EventArgs& e);
	bool HandleDragStarted(const CEGUI::EventArgs& e);
	bool HandleMouseEnterIconArea(const CEGUI::EventArgs& e);
	bool HandleDragEnded(const CEGUI::EventArgs& e);

	void Update();
	void UpdateDragContainer(CEGUI::Window& window);

private:
	CEGUI::Window*		  window_;
	CEGUI::PushButton*	  closeButton_;
	CEGUI::Scrollbar*	  scrollBar_;
	CEGUI::Window*		  iconSlots_[MAGIC_WHITE_SLOT_COUNT];
	CEGUI::Window*		  descriptionSlots_[MAGIC_WHITE_SLOT_COUNT];
	CEGUI::PushButton*	  auraButton_;
	CEGUI::PushButton*	  blueButton_;
	CEGUI::PushButton*	  blackButton_;
	CEGUI::PushButton*	  abilityButton_;
	CEGUI::DragContainer* dragContainer_;
	CEGUI::Window*		  dragItem_;
};

} // namespace GUI

#endif // GUI_MAGIC_WHITE_WINDOW_H