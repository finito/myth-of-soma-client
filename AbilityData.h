#ifndef ABILITY_DATA_H
#define ABILITY_DATA_H

#include <string>

namespace Network
{
	class GamePacket;
}

class AbilityData
{
public:
	AbilityData();

	void HandleReceive(Network::GamePacket& packet);

public:
	short		   id_;
	unsigned char  type_;
	std::string    name_;
	std::string    description_;
	unsigned short requirementSkill_;
	unsigned short useCount_;
	unsigned short maximumUseCount_;
};

#endif // ABILITY_DATA_H