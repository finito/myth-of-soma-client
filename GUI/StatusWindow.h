#ifndef GUI_STATUS_WINDOW_H
#define GUI_STATUS_WINDOW_H

#include <CEGUI/CEGUI.h>
#include "Moral.h"

namespace GUI
{

class StatusWindow
{
public:
	StatusWindow();
	~StatusWindow();

private:
	void Update();

	bool HandleOpen(const CEGUI::EventArgs& e);
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleWindowUpdated(const CEGUI::EventArgs& e);

private:
	CEGUI::Window*     window_;
	CEGUI::Window*     name_;
	CEGUI::Window*     knownAs_;
	CEGUI::Window*     guildName_;
	CEGUI::Window*     pos_;
	CEGUI::Window*     level_;
	CEGUI::Window*     age_;
	CEGUI::Window*     gender_;
	CEGUI::Window*     moral_;
	CEGUI::Window*     health_;
	CEGUI::Window*     maximumHealth_;
	CEGUI::Window*     mana_;
	CEGUI::Window*     maximumMana_;
	CEGUI::Window*     stamina_;
	CEGUI::Window*     maximumStamina_;
	CEGUI::Window*     strength_;
	CEGUI::Window*     intelligence_;
	CEGUI::Window*     dexterity_;
	CEGUI::Window*     wisdom_;
	CEGUI::Window*     charisma_;
	CEGUI::Window*     constitution_;
	CEGUI::Window*     swordSkill_;
	CEGUI::Window*     spearSkill_;
	CEGUI::Window*     axeSkill_;
	CEGUI::Window*     knuckleSkill_;
	CEGUI::Window*     bowSkill_;
	CEGUI::Window*     staffSkill_;
	CEGUI::Window*     armorCraftSkill_;
	CEGUI::Window*     weaponCraftSkill_;
	CEGUI::Window*     accessoryCraftSkill_;
	CEGUI::Window*     potionCraftSkill_;
	CEGUI::Window*     cookingCraftSkill_;
	CEGUI::Window*     blackMagicSkill_;
	CEGUI::Window*     whiteMagicSkill_;
	CEGUI::Window*     blueMagicSkill_;
	CEGUI::PushButton* closeButton_;
	Moral			   moralValue_;
};

} // namespace GUI

#endif // GUI_STATUS_WINDOW_H