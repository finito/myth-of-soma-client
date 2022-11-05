#ifndef GUI_SELECT_CHARACTER_WINDOW_H
#define GUI_SELECT_CHARACTER_WINDOW_H

#include <CEGUI/CEGUI.h>

#include "GUI/Moral.h"
#include "GUI/CharacterRender.h"

namespace GUI
{

namespace RotateDirection
{
	enum Enum
	{
		Left,
		Right,
		First = Left,
		Last = Right + 1,
	};
}

class SelectCharacterWindow
{
public:
	struct CharacterData
	{
		std::string name_;
		short class_;
		short level_;
		short strength_;
		short dexterity_;
		short intelligence_;
		short charisma_;
		short wisdom_;
		short constitution_;
		short health_;
		short maximumHealth_;
		short mana_;
		short maximumMana_;
		short stamina_;
		short maximumStamina_;
		short age_;
		short moral_;
		short gender_;
		short hair_;
		short hairMode_;
		short skin_;
		std::string guildName_;
		short fame_;
		int swordExp_;
		int spearExp_;
		int bowExp_;
		int axeExp_;
		int knuckleExp_;
		int staffExp_;
		int weaponMakeExp_;
		int armorMakeExp_;
		int _accessoryMakeExp;
		int potionMakeExp_;
		int cookingExp_;
		short equippedItem_[10];
	};

	SelectCharacterWindow();
	~SelectCharacterWindow();

	void SetOKButtonEventHandler(CEGUI::Event::Subscriber subscriber);
	void SetExitButtonEventHandler(CEGUI::Event::Subscriber subscriber);

	void SetActiveCharacterSlot(int slot);
	void SetCharacterData(const CharacterData& characterData, int slot);
	void SetCharacterRender(CharacterRender& characterRender, const CharacterData& characterData);

	void HandleCharacterDataLoadComplete();

	const std::string GetActiveCharacterName() const;

private:
	bool HandleWindowUpdated(const CEGUI::EventArgs& e);
	bool HandleKeyPress(const CEGUI::EventArgs& e);

	void SetCharacterDataText();
	void ClearCharacterDataText();

	bool HandleRotateLeft(const CEGUI::EventArgs& e);
	bool HandleRotateRight(const CEGUI::EventArgs& e);
	bool HandleRotateLeftAnimationEnded(const CEGUI::EventArgs& e);
	bool HandleRotateRightAnimationEnded(const CEGUI::EventArgs& e);
	void HandleRotate(RotateDirection::Enum _direction);

private:
	CEGUI::Window*	   window_;
	CEGUI::PushButton* okButton_;
	CEGUI::PushButton* exitButton_;
	CEGUI::PushButton* humanWorldButton_;
	CEGUI::PushButton* devilWorldButton_;
	CEGUI::PushButton* deleteButton_;
	CEGUI::PushButton* selectServerButton_;
	CEGUI::PushButton* rotateLeftButton_;
	CEGUI::PushButton* rotateRightButton_;
	CEGUI::Window*     name_;
	CEGUI::Window*     guildName_;
	CEGUI::Window*     age_;
	CEGUI::Window*     position_;
	CEGUI::Window*     moral_;
	CEGUI::Window*     level_;
	CEGUI::Window*     health_;
	CEGUI::Window*     mana_;
	CEGUI::Window*     stamina_;
	CEGUI::Window*     mainWeapon_;
	CEGUI::Window*     mainSkill_;
	CEGUI::Window*     strength_;
	CEGUI::Window*     intelligence_;
	CEGUI::Window*     dexterity_;
	CEGUI::Window*     wisdom_;
	CEGUI::Window*     charisma_;
	CEGUI::Window*     constitution_;
	CEGUI::Window*     messageBox_;
	CEGUI::Window*     messageBoxOKCancel_;

	CharacterData characterData_[3];
	int			  activeCharacterSlot_;
	int			  characterCount_;
	int			  characterRenderSlot_[3];

	Moral moralValue_;

	//CharacterRender	characterRender1_;
	//CharacterRender characterRender2_;
	//CharacterRender	characterRender3_;
};

} // namespace GUI

#endif // GUI_SELECT_CHARACTER_WINDOW_H