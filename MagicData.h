#ifndef MAGIC_DATA_H
#define MAGIC_DATA_H

#include <string>

namespace MagicType
{
	const unsigned char Blue  = 1;
	const unsigned char White = 2;
	const unsigned char Black = 3;
}

namespace Network
{
	class GamePacket;
}

class MagicData
{
public:
	MagicData();

	void HandleReceive(Network::GamePacket& packet);

public:
	short		   id_;
	unsigned char  method_;
	unsigned char  characterOrAxis_;
	unsigned short startTime_;
	unsigned char  type_;
	unsigned char  targetMethod_;
	std::string    name_;
	std::string    description_;
	unsigned short range_;
	unsigned short manaCost_;
	unsigned int   duration_;
	short		   damage_;
	unsigned short requirementSkill_;
	unsigned short requirementIntelligence_;
	unsigned short requirementLevel_;
};

#endif // MAGIC_DATA_H