
#include "Components/ItemAttributeComponent.h"
#include "Network/GameProtocol.h"
#include "Network/GamePacket.h"

using namespace artemis;

ItemAttributeComponent::ItemAttributeComponent() :
index_(0), type_(ItemType::Sword), gender_(ItemGender::UniSex), pictureId_(0), minimumDamage_(0), maximumDamage_(0), weight_(0),
requirementStr_(0), requirementInt_(0), requirementDex_(0), requirementSkill_(0), money_(0), dura_(0), maximumDura_(0),
originalMaximumDura_(0), speed_(0), classType_(ItemClassType::Human), health_(0), mana_(0), shopQuantity_(0), id_(0),
equipPosition_(ItemEquipPosition::Head), upgradeLevel_(0), repairCost_(0), repairDuraAfter_(0)
{
	for (int i = 0; i < SPECIAL_COUNT; ++i)
	{
		specialData_[i].id = ItemSpecial::None;
		for (int j = 0; j < SPECIAL_OPTION_COUNT; ++j)
		{
			specialData_[i].options[j] = 0;
		}
	}

	for (int i = 0; i < REQUIRED_ITEM_COUNT; ++i)
	{
		requiredItemQuantities_[i] = 0;
	}
}

unsigned short ItemAttributeComponent::GetIndex() const
{
	return index_;
}

void ItemAttributeComponent::SetIndex(unsigned short value)
{
	index_ = value;
}

ItemType::Enum ItemAttributeComponent::GetType() const
{
	return type_;
}

void ItemAttributeComponent::SetType(ItemType::Enum value)
{
	type_ = value;
}

void ItemAttributeComponent::SetType(unsigned char value)
{
	// FIX !Assumes that the cast will always be valid!
	type_ = static_cast<ItemType::Enum>(value);
}

ItemGender::Enum ItemAttributeComponent::GetGender() const
{
	return gender_;
}

void ItemAttributeComponent::SetGender(ItemGender::Enum value)
{
	gender_ = value;
}

unsigned short ItemAttributeComponent::GetPictureId() const
{
	return pictureId_;
}

void ItemAttributeComponent::SetPictureId(unsigned short value)
{
	pictureId_ = value;
}

unsigned short ItemAttributeComponent::GetMinimumDamage() const
{
	return minimumDamage_;
}

void ItemAttributeComponent::SetMinimumDamage(unsigned short value)
{
	minimumDamage_ = value;
}

unsigned short ItemAttributeComponent::GetMaximumDamage() const
{
	return maximumDamage_;
}

void ItemAttributeComponent::SetMaximumDamage(unsigned short value)
{
	maximumDamage_ = value;
}

unsigned short ItemAttributeComponent::GetWeight() const
{
	return weight_;
}

void ItemAttributeComponent::SetWeight(unsigned short value)
{
	weight_ = value;
}

unsigned short ItemAttributeComponent::GetRequirementStr() const
{
	return requirementStr_;
}

void ItemAttributeComponent::SetRequirementStr(unsigned short value)
{
	requirementStr_ = value;
}

unsigned short ItemAttributeComponent::GetRequirementInt() const
{
	return requirementInt_;
}

void ItemAttributeComponent::SetRequirementInt(unsigned short value)
{
	requirementInt_ = value;
}

unsigned short ItemAttributeComponent::GetRequirementDex() const
{
	return requirementDex_;
}

void ItemAttributeComponent::SetRequirementDex(unsigned short value)
{
	requirementDex_ = value;
}

unsigned short ItemAttributeComponent::GetRequirementSkill() const
{
	return requirementSkill_;
}

void ItemAttributeComponent::SetRequirementSkill(unsigned short value)
{
	requirementSkill_ = value;
}

unsigned int ItemAttributeComponent::GetMoney() const
{
	return money_;
}

void ItemAttributeComponent::SetMoney(unsigned int value)
{
	money_ = value;
}

std::string ItemAttributeComponent::GetName() const
{
	return name_;
}

void ItemAttributeComponent::SetName(const std::string& name)
{
	name_ = name;
}

unsigned short ItemAttributeComponent::GetDura() const
{
	return dura_;
}

void ItemAttributeComponent::SetDura(unsigned short value)
{
	dura_ = value;
}

unsigned short ItemAttributeComponent::GetMaximumDura() const
{
	return maximumDura_;
}
void ItemAttributeComponent::SetMaximumDura(unsigned short value)
{
	maximumDura_ = value;
}

unsigned short ItemAttributeComponent::GetOriginalMaximumDura() const
{
	return originalMaximumDura_;
}
void ItemAttributeComponent::SetOriginalMaximumDura(unsigned short value)
{
	originalMaximumDura_ = value;
}

unsigned short ItemAttributeComponent::GetSpeed() const
{
	return speed_;
}

void ItemAttributeComponent::SetSpeed(unsigned short value)
{
	speed_ = value;
}

ItemClassType::Enum ItemAttributeComponent::GetClassType() const
{
	return classType_;
}

void ItemAttributeComponent::SetClassType(ItemClassType::Enum value)
{
	classType_ = value;
}
	
unsigned short ItemAttributeComponent::GetHealth() const
{
	return health_;
}

void ItemAttributeComponent::SetHealth(unsigned short value)
{
	health_ = value;
}

unsigned short ItemAttributeComponent::GetMana() const
{
	return mana_;
}

void ItemAttributeComponent::SetMana(unsigned short value)
{
	mana_ = value;
}

short ItemAttributeComponent::GetShopQuantity() const
{
	return shopQuantity_;
}

void ItemAttributeComponent::SetShopQuantity(short value)
{
	shopQuantity_ = value;
}

unsigned short ItemAttributeComponent::GetId() const
{
	return id_;
}

void ItemAttributeComponent::SetId(unsigned short value)
{
	id_ = value;
}

ItemEquipPosition::Enum ItemAttributeComponent::GetEquipPosition() const
{
	return equipPosition_;
}

void ItemAttributeComponent::SetEquipPosition(ItemEquipPosition::Enum value)
{
	equipPosition_ = value;
}

unsigned char ItemAttributeComponent::GetUpgradeLevel() const
{
	return upgradeLevel_;
}

void ItemAttributeComponent::SetUpgradeLevel(unsigned char value)
{
	upgradeLevel_ = value;
}

ItemSpecial::Enum ItemAttributeComponent::GetSpecialId(int index) const
{
	return specialData_[index].id;
}

void ItemAttributeComponent::SetSpecialId(int index, short value)
{
	// FIX !Assumes that the cast will always be valid!
	specialData_[index].id = static_cast<ItemSpecial::Enum>(value);
}

short ItemAttributeComponent::GetSpecialOption(int index, int optionIndex) const
{
	return specialData_[index].options[optionIndex];
}

void ItemAttributeComponent::SetSpecialOption(int index, int optionIndex, short value)
{
	specialData_[index].options[optionIndex] = value;
}

bool ItemAttributeComponent::Empty() const
{
	return dura_ == 0 ? true : false;
}

void ItemAttributeComponent::HandleItemReceive(Network::GamePacket& packet, const unsigned char type, const bool receiveIndex)
{
	if (receiveIndex)
	{
		sf::Uint16 index;
		packet >> index;
		SetIndex(index);
	}

	if (type & Network::GameProtocol::SHOP_LAYOUT)
	{
		sf::Uint16 id;
		packet >> id;
		SetId(id);

		sf::Int16 quantity;
		packet >> quantity;
		SetShopQuantity(quantity);
	}

	if (type & Network::GameProtocol::AUCTION_LAYOUT)
	{
		sf::Uint16 money2;
		packet >> money2;

		for (int i = 0; i < REQUIRED_ITEM_COUNT; ++i)
		{
			std::string requiredItemName;
			packet >> requiredItemName;
			SetRequiredItemName(requiredItemName, i);
			if (!requiredItemName.empty())
			{
				sf::Uint16 requiredItemQuantity;
				packet >> requiredItemQuantity;
				SetRequiredItemQuantity(requiredItemQuantity, i);
			}
			else
			{
				SetRequiredItemQuantity(0, i);
			}
		}

		sf::Uint16 id;
		packet >> id;
		SetId(id);
	}

	sf::Uint8 itemType;
	packet >> itemType;
	SetType(itemType);

	if (!(type & Network::GameProtocol::SHOP_LAYOUT) && !(type & Network::GameProtocol::AUCTION_LAYOUT))
	{
		sf::Uint8 equipPosition;
		packet >> equipPosition;
		// FIX !Assumes that the cast will always be valid!
		SetEquipPosition(static_cast<ItemEquipPosition::Enum>(equipPosition));
	}

	sf::Uint16 classType;
	packet >> classType;
	SetClassType(classType == 10 ? ItemClassType::Devil : ItemClassType::Human);

	sf::Uint16 pictureId;
	packet >> pictureId;
	this->pictureId_ = pictureId;

	if (type & Network::GameProtocol::TRADE_LAYOUT)
	{
		sf::Uint16 dura;
		packet >> dura;
		SetDura(dura);
	}

	sf::Uint8 gender;
	packet >> gender;
	// FIX !Assumes that the cast will always be valid!
	SetGender(static_cast<ItemGender::Enum>(gender));

	sf::Uint16 minimumDamage;
	packet >> minimumDamage;
	SetMinimumDamage(minimumDamage);

	sf::Uint16 maximumDamage;
	packet >> maximumDamage;
	SetMaximumDamage(maximumDamage);

	sf::Uint16 weight;
	packet >> weight;
	SetWeight(weight);

	sf::Uint16 speed;
	packet >> speed;
	SetSpeed(speed);

	if (type & Network::GameProtocol::GET_VALUE)
	{
		sf::Uint32 money;
		packet >> money;
		SetMoney(money);
	}

	sf::Uint16 requirementStr;
	packet >> requirementStr;
	SetRequirementStr(requirementStr);

	sf::Uint16 requirementInt;
	packet >> requirementInt;
	SetRequirementInt(requirementInt);

	sf::Uint16 requirementDex;
	packet >> requirementDex;
	SetRequirementDex(requirementDex);

	sf::Uint16 requirementSkill;
	packet >> requirementSkill;
	SetRequirementSkill(requirementSkill);

	sf::Uint16 health;
	packet >> health;
	SetHealth(health);

	sf::Uint16 mana;
	packet >> mana;
	SetMana(mana);

	if (type & Network::GameProtocol::NORM_LAYOUT)
	{
		sf::Uint16 dura;
		packet >> dura;
		SetDura(dura);
	}

	if (type & Network::GameProtocol::SHOP_LAYOUT || type & Network::GameProtocol::AUCTION_LAYOUT)
	{
		sf::Uint32 money;
		packet >> money;
		SetMoney(money);
	}

	if (type & Network::GameProtocol::GET_EXTRA_USAGE)
	{
		sf::Uint16 maximumDura;
		packet >> maximumDura;
		SetMaximumDura(maximumDura);

		sf::Uint16 originalMaximumDura;
		packet >> originalMaximumDura;
		SetOriginalMaximumDura(originalMaximumDura);
	}

	std::string name;
	packet >> name;
	SetName(name);

	sf::Uint8 upgradeLevel;
	packet >> upgradeLevel;
	SetUpgradeLevel(upgradeLevel);

	for (int i = 0; i < SPECIAL_COUNT; ++i)
	{
		sf::Uint16 specialId;
		packet >> specialId;
		SetSpecialId(i, specialId);
		for (int j = 0; j < SPECIAL_OPTION_COUNT; ++j)
		{
			sf::Uint16 specialOption;
			packet >> specialOption;
			SetSpecialOption(i, j, specialOption);
		}
	}
}

void ItemAttributeComponent::HandleChangeItemInfo(Network::GamePacket& packet, const unsigned char type)
{
	if (type & Network::GameProtocol::INFO_TYPE)
	{
		sf::Uint8 itemType;
		packet >> itemType;
		SetType(itemType);

		sf::Uint8 gender;
		packet >> gender;
		// FIX !Assumes that the cast will always be valid!
		SetGender(static_cast<ItemGender::Enum>(gender));
	}
	if(type & Network::GameProtocol::INFO_ARM)
	{
		sf::Uint8 equipPosition;
		packet >> equipPosition;
		// FIX !Assumes that the cast will always be valid!
		SetEquipPosition(static_cast<ItemEquipPosition::Enum>(equipPosition));

		sf::Uint16 weight;
		packet >> weight;
		SetWeight(weight);
		
		sf::Uint16 speed;
		packet >> speed;
		SetSpeed(speed);

		sf::Uint16 classType;
		packet >> classType;
		SetClassType(classType == 10 ? ItemClassType::Devil : ItemClassType::Human);
	}
	if(type & Network::GameProtocol::INFO_PICNUM)
	{
		sf::Uint16 pictureId;
		packet >> pictureId;
		SetPictureId(pictureId);
	}
	if(type & Network::GameProtocol::INFO_DUR)
	{
		sf::Uint16 dura;
		packet >> dura;
		SetDura(dura);
	}
	if(type & Network::GameProtocol::INFO_NAME)
	{
		std::string name;
		packet >> name;
		SetName(name);

		sf::Uint8 upgradeLevel;
		packet >> upgradeLevel;
		SetUpgradeLevel(upgradeLevel);
	}
	if(type & Network::GameProtocol::INFO_DAMAGE)
	{
		sf::Uint16 minimumDamage;
		packet >> minimumDamage;
		SetMinimumDamage(minimumDamage);

		sf::Uint16 maximumDamage;
		packet >> maximumDamage;
		SetMaximumDamage(maximumDamage);
	}
	if(type & Network::GameProtocol::INFO_LIMIT)
	{
		sf::Uint16 maximumDura;
		packet >> maximumDura;
		SetMaximumDura(maximumDura);

		sf::Uint16 originalMaximumDura;
		packet >> originalMaximumDura;
		SetOriginalMaximumDura(originalMaximumDura);

		sf::Uint32 money;
		packet >> money;
		SetMoney(money);

		sf::Uint16 requirementStr;
		packet >> requirementStr;
		SetRequirementStr(requirementStr);

		sf::Uint16 requirementInt;
		packet >> requirementInt;
		SetRequirementInt(requirementInt);

		sf::Uint16 requirementDex;
		packet >> requirementDex;
		SetRequirementDex(requirementDex);

		sf::Uint16 requirementSkill;
		packet >> requirementSkill;
		SetRequirementSkill(requirementSkill);
	}
	if(type & Network::GameProtocol::INFO_SPECIAL)
	{
		sf::Uint16 health;
		packet >> health;
		SetHealth(health);

		sf::Uint16 mana;
		packet >> mana;
		SetMana(mana);

		for (int i = 0; i < SPECIAL_COUNT; ++i)
		{
			sf::Uint16 specialId;
			packet >> specialId;
			SetSpecialId(i, specialId);
			for (int j = 0; j < SPECIAL_OPTION_COUNT; ++j)
			{
				sf::Uint16 specialOption;
				packet >> specialOption;
				SetSpecialOption(i, j, specialOption);
			}
		}
	}
}

unsigned int ItemAttributeComponent::GetQuantity() const
{
	if (GetDura() > 1 && GetType() >= ItemType::Potion && GetType() <= ItemType::Quest)
	{
		return GetDura();
	}
	return 1;
}

unsigned int ItemAttributeComponent::GetRepairCost() const
{
	return repairCost_;
}

void ItemAttributeComponent::SetRepairCost(const unsigned int value)
{
	repairCost_ = value;
}

unsigned int ItemAttributeComponent::GetRepairDuraAfter() const
{
	return repairDuraAfter_;
}

void ItemAttributeComponent::SetRepairDuraAfter(const unsigned int value)
{
	repairDuraAfter_ = value;
}

std::string ItemAttributeComponent::GetRequiredItemName(const unsigned int index) const
{
	return index >= 0 && index < 5 ? requiredItemNames_[index] : "";
}

void ItemAttributeComponent::SetRequiredItemName(const std::string& value, const unsigned int index)
{
	if (index >= 0 && index < 5)
	{
		requiredItemNames_[index] = value;
	}
}

unsigned int ItemAttributeComponent::GetRequiredItemQuantity(const unsigned int index) const
{
	return index >= 0 && index < 5 ? requiredItemQuantities_[index] : 0;
}

void ItemAttributeComponent::SetRequiredItemQuantity(const unsigned int value, const unsigned int index)
{
	if (index >= 0 && index < 5)
	{
		requiredItemQuantities_[index] = value;
	}
}

bool operator <(const ItemAttributeComponent& lhs, const ItemAttributeComponent& rhs)
{
	// Potions / Food / Materials go at the top! followed by Weapons, Armor, Accessory, Quest, Token

	// Check if type of item are same then order by ArmPos or Name
	if (lhs.GetType() == rhs.GetType())
	{
		// Order items by their arm position e.g. 6 = Armor 7 = Shield etc.
		if (lhs.GetEquipPosition() != rhs.GetEquipPosition())
		{
			if (lhs.GetEquipPosition() > rhs.GetEquipPosition())
			{
				return false;
			}
			else if (lhs.GetEquipPosition() < rhs.GetEquipPosition())
			{
				return true;
			}
		}

		// Order items by their name
		if (lhs.GetName() > rhs.GetName())
		{
			return false;
		}
		else if (lhs.GetName() < rhs.GetName())
		{
			return true;
		}
		
		return false;
	}
	else if ((lhs.GetType() >= ItemType::Potion && lhs.GetType() <= ItemType::Material) &&
		(rhs.GetType() >= ItemType::Potion && rhs.GetType() <= ItemType::Material)) // Order potions/food/mat
	{
		if (lhs.GetType() > rhs.GetType())
		{
			return false;
		}
		else if (lhs.GetType() < rhs.GetType())
		{
			return true;
		}
	}
	else if (lhs.GetType() <= ItemType::Staff && rhs.GetType() <= ItemType::Staff) // Order weapons by the type 
	{
		if (lhs.GetType() > rhs.GetType())
		{
			return false;
		}
		else if (lhs.GetType() < rhs.GetType())
		{
			return true;
		}
	}
	else if ((lhs.GetType() >= ItemType::Armor && lhs.GetType() <= ItemType::Accessory) &&
		(rhs.GetType() >= ItemType::Armor && rhs.GetType() <= ItemType::Accessory)) // Order Armor by the type
	{
		if (lhs.GetType() > rhs.GetType())
		{
			return false;
		}
		else if (lhs.GetType() < rhs.GetType())
		{
			return true;
		}
	}
	else if ((lhs.GetType() >= ItemType::Potion && lhs.GetType() <= ItemType::Material) &&
		(rhs.GetType() <= ItemType::Accessory || rhs.GetType() >= ItemType::Token)) // Potions before all other items
	{
		return true;
	}
	else if ((lhs.GetType() <= ItemType::Accessory || lhs.GetType() >= ItemType::Token) &&
		(rhs.GetType() >= ItemType::Potion && rhs.GetType() <= ItemType::Material)) // Potions before all other items
	{
		return false;
	}
	else if (lhs.GetType() <= ItemType::Staff &&
		(rhs.GetType() >= ItemType::Armor && rhs.GetType() <= ItemType::Accessory)) // Weapons before Armors
	{
		return true;
	}
	else if ((lhs.GetType() >= ItemType::Armor && lhs.GetType() <= ItemType::Accessory) &&
		rhs.GetType() <= ItemType::Staff) // Weapons before Armors
	{
		return false;
	}
	else if (lhs.GetType() <= ItemType::Accessory && rhs.GetType() >= ItemType::Token) // Armors before Quest / Token
	{
		return true;
	}
	else if (lhs.GetType() == ItemType::Quest && rhs.GetType() == ItemType::Token) // Quest before Token
	{
		return true;
	}
	else if (lhs.GetType() == ItemType::Token && rhs.GetType() == ItemType::Quest) // Quest before Token
	{
		return false;
	}
		
	return false;
}