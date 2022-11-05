#ifndef COMPONENTS_ITEM_ATTRIBUTE_COMPONENT_H
#define COMPONENTS_ITEM_ATTRIBUTE_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <string>

namespace Network
{
	class GamePacket;
}

namespace ItemType
{
	enum Enum
	{
		Sword,
		Axe,
		Bow,
		Spear,
		Knuckle,
		Staff,
		Weapon,	// Is not used
		Armor,
		Accessory,
		Potion,
		Food,
		Material,
		Token,
		Money,
		Unknown,
		Quest,
		First = Sword,
		Last = Quest + 1
	};
}

namespace ItemEquipPosition
{
	enum Enum
	{
		Head,
		Ear,
		Neck,
		Chest,
		Leg,
		Foot,
		RightHand,
		LeftHand,
		BothHands,
		Wrists,
		Use,
		Con,
		First = Head,
		Last = Con + 1
	};	
}

namespace ItemMaterialType
{
	enum Enum
	{
		Metal = 1,
		Wood,
		Leather,
		Bone,
		Cloth,
		Plant,
		Meat,
		Powder,
		Spice,
		Gem,
		Etc,
		First = Metal,
		Last = Etc + 1
	};	
}

namespace ItemGender
{
	enum Enum
	{
		UniSex,
		Female,
		Male
	};
}

namespace ItemClassType
{
	enum Enum
	{
		Human = 0,
		Devil = 10,
	};
}

namespace ItemSpecial
{
	enum Enum
	{
		None,
		Color,
		RunEvent,
		Light,
		Antidote,
		Portal,
		TownPortal,
		MoralAdd,
		MagicAttack,
		MagicDefense,
		Unknown, // beep
		Strength,
		Intelligence,
		Dexterity,
		Wisdom,
		Constitution,
		Charisma, // beep2
		MaximumHealth,
		MaximumMana,
		MaximumStamina,
		MaximumWeight,
		PhysicalAttack,
		PhysicalDefense,
		MagicAttack2,
		MagicDefense2,
		BlueMagicAttack,
		WhiteMagicAttack,
		BlackMagicAttack,
		BlueMagicDefense,
		WhiteMagicDefense,
		BlackMagicDefense,
		Compass,
		PosGem,
		RandomTeleport,
		StatModifierHealth,
		StatModifierMana,
		StatModifierStamina,
		StatModifierWeight,
		Lottery,
		Rangechange,
		StaminaRecover,
		DetoxBlue,
		DetoxWhite,
		DetoxBlack,
		Revive,
		ResistPoison,
		MagicAvailable,
		Evasion,
		Repair,
		Unknown3,
		Smelting,
		Unknown4,
		Cooking,
		Upgrade,
		SpecialRepair,
		Accuracy,
		FortunePouch,
		Plus,
		GenderChange,
		SkinChange,
		Shout,
		SpecialGem,
		SingleShout,
		HairDye
	};
	
	inline const char* ToString(Enum v)
	{
		switch (v)
		{
		case Antidote:
			return "Antidote against Normal Poison";
		case MagicAttack:
			return "Magic Attack : %1%~%2%";
		case MagicDefense:
			return "Magic Defense : %1%~%2%";
		case Strength:
			return "STR+%1%";
		case Intelligence:
			return "INT+%1%";
		case Dexterity:
			return "DEX+%1%";
		case Wisdom:
			return "WIS+%1%";
		case Constitution:
			return "CON+%1%";
		case Charisma:
			return "CHA+%1%";
		case MaximumHealth:
			return "Max HP+%1%";
		case MaximumMana:
			return "Max MP+%1%";
		case MaximumStamina:
			return "Max Stamina+%1%";
		case MaximumWeight:
			return "Max Weight+%1%";
		case PhysicalAttack:
			return "Attack+%1%";
		case PhysicalDefense:
			return "Defense+%1%";
		case MagicAttack2:
			return "Magic Attack+%1%";
		case MagicDefense2:
			return "Magic Defense+%1%";
		case BlueMagicAttack:
			return "Blue Magic Attack+%1%";
		case WhiteMagicAttack:
			return "White Magic Attack+%1%";
		case BlackMagicAttack:
			return "Black Magic Attack+%1%";
		case BlueMagicDefense:
			return "Blue Magic Defense+%1%";
		case WhiteMagicDefense:
			return "White Magic Defense+%1%";
		case BlackMagicDefense:
			return "Black Magic Defense+%1%";
		case StatModifierHealth:
			return "Max HP Up+%1%";
		case StatModifierMana:
			return "Max MP Up+%1%";
		case StatModifierStamina:
			return "Max Stamina Up+%1%";
		case StatModifierWeight:
			return "Max Weight Up+%1%";
		case StaminaRecover:
			return "Stamina Recovery+%1%";
		case DetoxBlue:
			return "Detox Blue Magic";
		case DetoxWhite:
			return "Detox White Magic";
		case DetoxBlack:
			return "Detox Black Magic";
		case Revive:
			return "Revive";
		case ResistPoison:
			return "Resist Poison";
		case MagicAvailable:
			return "Magic Available";
		case Evasion:
			return "Modified Evasion Rate+%1%%%";
		case Cooking:
			return "For cooking (Restriction D)";
		case Accuracy:
			return "Modified Accuracy Rate+%1%%%";
		case GenderChange:
			return "Gender Change";
		default:
			return "";
		}
	}
}

class ItemAttributeComponent : public artemis::Component
{
public:
	static const int SPECIAL_COUNT = 3;
	static const int SPECIAL_OPTION_COUNT = 3;
	static const int REQUIRED_ITEM_COUNT = 5;

	ItemAttributeComponent();

	unsigned short GetIndex() const;
	void SetIndex(unsigned short value);

	ItemType::Enum GetType() const;
	void SetType(ItemType::Enum value);
	void SetType(unsigned char value);

	ItemGender::Enum GetGender() const;
	void SetGender(ItemGender::Enum value);

	unsigned short GetPictureId() const;
	void SetPictureId(unsigned short value);

	unsigned short GetMinimumDamage() const;
	void SetMinimumDamage(unsigned short value);

	unsigned short GetMaximumDamage() const;
	void SetMaximumDamage(unsigned short value);

	unsigned short GetWeight() const;
	void SetWeight(unsigned short value);

	unsigned short GetRequirementStr() const;
	void SetRequirementStr(unsigned short value);

	unsigned short GetRequirementInt() const;
	void SetRequirementInt(unsigned short value);

	unsigned short GetRequirementDex() const;
	void SetRequirementDex(unsigned short value);

	unsigned short GetRequirementSkill() const;
	void SetRequirementSkill(unsigned short value);

	unsigned int GetMoney() const;
	void SetMoney(unsigned int value);

	std::string GetName() const;
	void SetName(const std::string& name);

	unsigned short GetDura() const;
	void SetDura(unsigned short value);

	unsigned short GetMaximumDura() const;
	void SetMaximumDura(unsigned short value);

	unsigned short GetOriginalMaximumDura() const;
	void SetOriginalMaximumDura(unsigned short value);

	unsigned short GetSpeed() const;
	void SetSpeed(unsigned short value);

	ItemClassType::Enum GetClassType() const;
	void SetClassType(ItemClassType::Enum value);
	
	unsigned short GetHealth() const;
	void SetHealth(unsigned short value);

	unsigned short GetMana() const;
	void SetMana(unsigned short value);

	short GetShopQuantity() const;
	void SetShopQuantity(short value);

	unsigned short GetId() const;
	void SetId(unsigned short value);

	ItemEquipPosition::Enum GetEquipPosition() const;
	void SetEquipPosition(ItemEquipPosition::Enum value);

	unsigned char GetUpgradeLevel() const;
	void SetUpgradeLevel(unsigned char value);

	ItemSpecial::Enum GetSpecialId(int index) const;
	void SetSpecialId(int index, short value);

	short GetSpecialOption(int index, int optionIndex) const;
	void SetSpecialOption(int index, int optionIndex, short value);

	bool Empty() const;

	unsigned int GetQuantity() const;

	unsigned int GetRepairCost() const;
	void SetRepairCost(unsigned int value);

	unsigned int GetRepairDuraAfter() const;
	void SetRepairDuraAfter(unsigned int value);

	std::string GetRequiredItemName(unsigned int index) const;
	void SetRequiredItemName(const std::string& value, unsigned int index);

	unsigned int GetRequiredItemQuantity(unsigned int index) const;
	void SetRequiredItemQuantity(unsigned int value, unsigned int index);

	// TODO: Put these functions somewhere else.
	void HandleChangeItemInfo(Network::GamePacket& packet, unsigned char type);
	void HandleItemReceive(Network::GamePacket& packet, unsigned char type, bool receiveIndex = true);

private:
	struct SpecialData
	{
		ItemSpecial::Enum id; // Special no e.g. pos gem / shout scroll / compass
		short options[SPECIAL_OPTION_COUNT];
	};

	unsigned short index_;					// A slot number of where the item is placed.
	ItemType::Enum type_;
	ItemGender::Enum gender_;				// Item Gender, 0 = GenderLess, 1 = woman, 2 = man
	unsigned short pictureId_;
	unsigned short minimumDamage_;
	unsigned short maximumDamage_;
	unsigned short weight_;
	unsigned short requirementStr_;
	unsigned short requirementInt_;
	unsigned short requirementDex_;
	unsigned short requirementSkill_;
	unsigned int   money_;					// Value of the item, Only used in shops and auction/Guild shop
	std::string	   name_;
	unsigned short dura_;					// Current Duration / Amount of an item such as pots, dependant on whether it is stackable
	unsigned short maximumDura_;			// Current Duration (MAX)
	unsigned short originalMaximumDura_;	// Original Duration (MAX)
	unsigned short speed_;					// Speed of the weapon
	ItemClassType::Enum classType_;			// Class of the item. 0 = human, 10 = devil
	unsigned short health_;					// Health restore amount
	unsigned short mana_;					// Mana restore amount
	short		   shopQuantity_;				// Represent the amount of items left in stock at shops or quantity of item placed on zone.
	unsigned short id_;						// Used by shops when updating quantity
	ItemEquipPosition::Enum equipPosition_;	// Which equipment slot the item will go into if it can be equipped.
	//std::string  auctionRegistrant;		// Name of the player that registered the item for auction.
	//std::string  auctionHighestBidder;	// Name of the player that currently has the highest bid of the item for auction.
	unsigned char  upgradeLevel_;			// Upgrade lvl of item... has + X next to name
	SpecialData	   specialData_[SPECIAL_COUNT];
	unsigned int   repairCost_;
	unsigned int   repairDuraAfter_;
	std::string    requiredItemNames_[REQUIRED_ITEM_COUNT];
	unsigned int   requiredItemQuantities_[REQUIRED_ITEM_COUNT];
};

bool operator <(const ItemAttributeComponent& lhs, const ItemAttributeComponent& rhs);

#endif // COMPONENTS_ITEM_ATTRIBUTE_COMPONENT_H