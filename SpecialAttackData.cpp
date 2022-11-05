
#include "SpecialAttackData.h"
#include "Network/GamePacket.h"

SpecialAttackData::SpecialAttackData() :
id_(-1),
targetMethod_(0),
range_(0),
healthCost_(0),
manaCost_(0),
staminaCost_(0),
duration_(0),
cooldown_(0),
damage_(0),
requirementSkill_(0),
weaponType_(0),
changeTime_(0),
healthChange_(0),
manaChange_(0)
{
}

void SpecialAttackData::HandleReceive(Network::GamePacket& packet)
{
	sf::Int16 id;
	packet >> id;
	id_ = id;

	sf::Uint8 targetMethod;
	packet >> targetMethod;
	targetMethod_ = targetMethod;

	packet >> name_;

	packet >> description_;

	sf::Uint16 range;
	packet >> range;
	range_ = range;

	sf::Uint16 healthCost;
	packet >> healthCost;
	healthCost_ = healthCost;

	sf::Uint16 manaCost;
	packet >> manaCost;
	manaCost_ = manaCost;

	sf::Uint16 staminaCost;
	packet >> staminaCost;
	staminaCost_ = staminaCost;

	sf::Uint32 duration;
	packet >> duration;
	duration_ = duration;

	sf::Uint32 cooldown;
	packet >> cooldown;
	cooldown_ = cooldown;

	sf::Int16 damage;
	packet >> damage;
	damage_ = damage;

	sf::Uint16 requirementSkill;
	packet >> requirementSkill;
	requirementSkill_ = requirementSkill;

	sf::Uint8 weaponType;
	packet >> weaponType;
	weaponType_ = weaponType;

	sf::Uint32 changeTime;
	packet >> changeTime;
	changeTime_ = changeTime;

	sf::Uint16 healthChange;
	packet >> healthChange;
	healthChange_ = healthChange;

	sf::Uint16 manaChange;
	packet >> manaChange;
	manaChange_ = manaChange;

	// TODO: Find out what this actually is, for now just skipping over it.
	sf::Uint16 unknown;
	packet >> unknown;
}

const std::string SpecialAttackData::WeaponTypeToString() const
{
	std::string weaponTypeName;

	switch (weaponType_)
	{
	case SpecialAttackWeaponType::Sword:
		weaponTypeName = "Sword";
		break;
	case SpecialAttackWeaponType::Axe:
		weaponTypeName = "Axe";
		break;
	case SpecialAttackWeaponType::Bow:
		weaponTypeName = "Bow";
		break;
	case SpecialAttackWeaponType::Spear:
		weaponTypeName = "Spear";
		break;
	case SpecialAttackWeaponType::Knuckle:
		weaponTypeName = "Knuckle";
		break;
	case SpecialAttackWeaponType::Staff:
		weaponTypeName = "Staff";
		break;
	}

	return weaponTypeName;
}