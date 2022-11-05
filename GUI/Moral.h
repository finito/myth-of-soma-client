#ifndef GUI_MORAL_H
#define GUI_MORAL_H

#include <CEGUI/CEGUI.h>

namespace GUI
{

// A class that can be used by the GUI
// as a way to get the text and colour 
// for a moral given it's value.
class Moral
{
public:
	Moral();

	void SetValue(int value);
	const CEGUI::String& GetText() const;
	const CEGUI::colour& GetColor() const;

private:
	int			  value_;
	CEGUI::String text_;
	CEGUI::colour color_;
};

} // namespace GUI

#endif GUI_MORAL_H