#ifndef ATTRIBUTE_COMPONENT_H
#define ATTRIBUTE_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Color.hpp>
#include <string>

namespace StateType
{
	enum Enum
	{
		Idle,
		Walking,
		Running,
		Attacking,
		Hit,
		Cast,
		Dead,
		First = Idle,
		Last  = Dead + 1,
	};
}

namespace BattleMode
{
	enum Enum
	{
		Normal,
		Battle,
		PK,
		First = Normal,
		Last  = PK + 1,
	};
}

namespace Gender
{
	enum Enum
	{
		Male,
		Female,
		First = Male,
		Last  = Female + 1,
	};
}

namespace MoralType
{
	enum Enum
	{
		Angel,
		Virtuous,
		Moral,
		Good,
		Neutral,
		Bad,
		Evil,
		Wicked,
		Devil,
		First = Angel,
		Last  = Devil + 1,
	};
}

namespace Direction
{
	enum Enum
	{
		Down,
		DownLeft,
		Left,
		UpLeft,
		Up,
		UpRight,
		Right,
		DownRight,
	};
}

class AttributeComponent : public artemis::Component
{
public:
	AttributeComponent();

	int GetServerId() const;
	void SetServerId(int value);

	Direction::Enum GetDirection() const;
	void SetDirection(Direction::Enum value);
	void SetDirection(unsigned char value);

	std::string GetGuildName() const;
	void SetGuildName(const std::string& value);

	short GetGuildId() const;
	void SetGuildId(short value);

	unsigned short GetGuildPictureId() const;
	void SetGuildPictureId(unsigned short value);

	std::string GetName() const;
	void SetName(const std::string& value);

	unsigned int GetSkin() const;
	void SetSkin(unsigned int value);

	unsigned int GetMoney() const;
	void SetMoney(unsigned int value);

	unsigned int GetAge() const;
	void SetAge(unsigned int value);

	MoralType::Enum GetMoral() const;
	void SetMoral(MoralType::Enum value);
	void SetMoral(int value);

	int GetMoralValue() const;
	void SetMoralValue(int value);

	Gender::Enum GetGender() const;
	void SetGender(Gender::Enum value);

	int GetPos() const;
	void SetPos(int value);

	unsigned short GetHealth() const;
	void SetHealth(unsigned short value);

	unsigned short GetMana() const;
	void SetMana(unsigned short value);

	unsigned short GetMaximumHealth() const;
	void SetMaximumHealth(unsigned short value);

	unsigned short GetMaximumMana() const;
	void SetMaximumMana(unsigned short value);

	unsigned short GetStr() const;
	void SetStr(unsigned short value);

	unsigned short GetDex() const;
	void SetDex(unsigned short value);

	unsigned short GetInt() const;
	void SetInt(unsigned short value);

	unsigned short GetWis() const;
	void SetWis(unsigned short value);

	unsigned short GetCha() const;
	void SetCha(unsigned short value);

	unsigned short GetCon() const;
	void SetCon(unsigned short value);

	unsigned short GetLevel() const;
	void SetLevel(unsigned short value);

	unsigned int GetExp() const;
	void SetExp(unsigned int value);

	unsigned int GetMaximumExp() const;
	void SetMaximumExp(unsigned int value);

	unsigned short GetWeight() const;
	void SetWeight(unsigned short value);

	unsigned short GetMaximumWeight() const;
	void SetMaximumWeight(unsigned short value);

	unsigned short GetStamina() const;
	void SetStamina(unsigned short value);
	void IncreaseStamina(unsigned short value);

	unsigned short GetMaximumStamina() const;
	void SetMaximumStamina(unsigned short value);

	unsigned int GetSwordExp() const;
	void SetSwordExp(unsigned int value);

	unsigned int GetSpearExp() const;
	void SetSpearExp(unsigned int value);

	unsigned int GetAxeExp() const;
	void SetAxeExp(unsigned int value);

	unsigned int GetKnuckleExp() const;
	void SetKnuckleExp(unsigned int value);

	unsigned int GetBowExp() const;
	void SetBowExp(unsigned int value);

	unsigned int GetStaffExp() const;
	void SetStaffExp(unsigned int value);

	unsigned int GetWeaponMakeExp() const;
	void SetWeaponMakeExp(unsigned int value);

	unsigned int GetArmorMakeExp() const;
	void SetArmorMakeExp(unsigned int value);

	unsigned int GetAccessoryMakeExp() const;
	void SetAccessoryMakeExp(unsigned int value);

	unsigned int GetPotionMakeExp() const;
	void SetPotionMakeExp(unsigned int value);

	unsigned int GetCookingExp() const;
	void SetCookingExp(unsigned int value);

	unsigned int GetBlackMagicExp() const;
	void SetBlackMagicExp(unsigned int value);

	unsigned int GetWhiteMagicExp() const;
	void SetWhiteMagicExp(unsigned int value);

	unsigned int GetBlueMagicExp() const;
	void SetBlueMagicExp(unsigned int value);

	StateType::Enum GetState() const;
	void SetState(StateType::Enum value);

	BattleMode::Enum GetBattleMode() const;
	void SetBattleMode(BattleMode::Enum  value);
	void SetBattleMode(unsigned char value);

	bool IsAlive() const;
	void SetAlive(bool value);

	bool IsAttackable() const;
	void SetAttackable(bool value);

	float GetHealthStatus() const;
	float GetManaStatus() const;
	float GetStaminaStatus() const;
	float GetWeightStatus() const;
	float GetExpStatus() const;

	unsigned short GetHair() const;
	void SetHair(unsigned short value);

	void SetRunToggle(bool value);
	bool GetRunToggle() const;

	void SetBloodType(int value);
	int GetBloodType() const;

	void SetPartyLeaderName(const std::string& value);
	std::string GetPartyLeaderName() const;

	void SetGrayMode(const unsigned char value);
	unsigned char GetGrayMode() const;
	bool IsGray() const;

	void RestartGrayTimer();
	const sf::Clock& GetGrayTimer() const;

	void SetFlashColor(sf::Color value);
	const sf::Color GetFlashColor() const;

	void RestartFlashColorTimer();
	const sf::Clock& GetFlashColorTimer() const;

	void SetClassType(unsigned short value);
	unsigned int GetClassType() const;

	std::map<unsigned short, artemis::Entity*>& GetMagicEntityMap();

	std::map<unsigned short, artemis::Entity&>& GetContinueEffectEntities();

private:
	int				serverId_;
	Direction::Enum direction_;
	std::string		name_;
	std::string		guildName_;
	short           guildId_;
	unsigned short  guildPictureId_;
	unsigned int	skin_;
	unsigned int	money_;
	unsigned int	age_;
	MoralType::Enum	moral_;
	int				moralValue_;
	Gender::Enum	gender_;
	int				pos_;
	unsigned short	health_;
	unsigned short	mana_;
	unsigned short	maximumHealth_;
	unsigned short	maximumMana_;
	unsigned short	str_;
	unsigned short	dex_;
	unsigned short	int_;
	unsigned short	wis_;
	unsigned short	cha_;
	unsigned short	con_;
	unsigned short	level_;
	unsigned int	exp_;
	unsigned int	maximumExp_;
	unsigned short	weight_;
	unsigned short	maximumWeight_;
	unsigned short	stamina_;
	unsigned short	maximumStamina_;

	// Weapon Skill Exp
	unsigned int swordExp_;
	unsigned int spearExp_;
	unsigned int axeExp_;
	unsigned int knuckleExp_;
	unsigned int bowExp_;
	unsigned int staffExp_;

	// Crafting Skill Exp
	unsigned int weaponMakeExp_;
	unsigned int armorMakeExp_;
	unsigned int accessoryMakeExp_;
	unsigned int potionMakeExp_;
	unsigned int cookingExp_;

	// Magic Skill Exp
	unsigned int blackMagicExp_;
	unsigned int whiteMagicExp_;
	unsigned int blueMagicExp_;

	StateType::Enum state_;

	BattleMode::Enum battleMode_;

	// Alive / Attackable status
	bool isAlive_;
	bool isAttackable_;

	unsigned short hair_;

	bool runToggle_;

	int bloodType_;

	std::string partyLeaderName_;

	unsigned char grayMode_;
	sf::Clock grayTimer_;

	sf::Color flashColor_;
	sf::Clock flashTimer_;

	unsigned short classType_;

	std::map<unsigned short, artemis::Entity*> magicEntityMap_;

	std::map<unsigned short, artemis::Entity&> magicEffectEntities_;
};

#endif // ATTRIBUTE_COMPONENT_H