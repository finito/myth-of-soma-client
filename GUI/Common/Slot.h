#ifndef GUI_SLOT_H
#define GUI_SLOT_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace GUI
{

class Slot : public CEGUI::Window
{
public:
	static const CEGUI::String EventNamespace;

	Slot(const CEGUI::String& type, const CEGUI::String& name);

	virtual ~Slot();

	void SetContents(const ItemAttributeComponent& item);
	//void SetContents(const MagicData& magic);
	//void SetContents(const SpecialAttackData& magic);
	//void SetContents(const AbilityData& magic);

private:
	CEGUI::Window& window_; // The window being used as the slot.
};

} // namespace GUI

#endif // GUI_SLOT_H