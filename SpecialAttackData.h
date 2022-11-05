#ifndef SPECIAL_ATTACK_DATA_H
#define SPECIAL_ATTACK_DATA_H

#include <string>

namespace SpecialAttackTargetMethod
{
	const unsigned char Arrow  = 1;
	const unsigned char Rail   = 2;
	const unsigned char Circle = 3;
}

namespace SpecialAttackWeaponType
{
	const unsigned char Sword	= 0;
	const unsigned char Axe		= 1;
	const unsigned char Bow		= 2;
	const unsigned char Spear	= 3;
	const unsigned char Knuckle	= 4;
	const unsigned char Staff	= 5;
}

namespace Network
{
	class GamePacket;
}

class SpecialAttackData
{
public:
	SpecialAttackData();

	void HandleReceive(Network::GamePacket& packet);

	const std::string WeaponTypeToString() const;

public:
	short		   id_;
	unsigned char  targetMethod_;
	std::string    name_;
	std::string    description_;
	unsigned short range_;
	unsigned short healthCost_;
	unsigned short manaCost_;
	unsigned short staminaCost_;
	unsigned int   duration_;
	unsigned int   cooldown_;
	short		   damage_;
	unsigned short requirementSkill_;
	unsigned char  weaponType_;
	unsigned int   changeTime_;
	short		   healthChange_;  // Can be both positive and negative number. Negative being used for regeneration.
	short		   manaChange_;
};

#endif // SPECIAL_ATTACK_DATA_H