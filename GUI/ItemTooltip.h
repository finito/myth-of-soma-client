#ifndef GUI_ITEM_TOOLTIP_H
#define GUI_ITEM_TOOLTIP_H

#include <CEGUI/CEGUI.h>

class ItemAttributeComponent;

namespace GUI
{

class ItemTooltip
{
public:
	ItemTooltip();
	~ItemTooltip();

	void Update(const ItemAttributeComponent& item);
	void Show(const CEGUI::UVector2& position);
	void Hide();

	void AppendRepairInfo(unsigned int duraAfter, unsigned int cost);

private:
	void SetClassType(const ItemAttributeComponent& item);
	void SetName(const ItemAttributeComponent& item);
	void SetStats(const ItemAttributeComponent& item);
	void SetRequirements(const ItemAttributeComponent& item);
	void SetSpecial(const ItemAttributeComponent& item);
	void SetRepairEffect(const ItemAttributeComponent& item);

private:
	CEGUI::Window* window_;
	CEGUI::Window* classType_;
	CEGUI::Window* name_;
	CEGUI::Window* stats_;
	CEGUI::Window* requirements_;
	CEGUI::Window* special_;
	CEGUI::Window* repairEffect_;
	CEGUI::Window* repairInfo_;
};

} // namespace GUI

#endif // GUI_ITEM_TOOLTIP_H