#include "AbilityData.h"
#include "Network/GamePacket.h"

AbilityData::AbilityData() :
id_(-1),
type_(0),
requirementSkill_(0),
useCount_(0),
maximumUseCount_(0)
{
}

void AbilityData::HandleReceive(Network::GamePacket& packet)
{
	sf::Int16 id;
	packet >> id;
	id_ = id;

	sf::Uint8 type;
	packet >> type;
	type_ = type;

	packet >> name_;

	packet >> description_;

	sf::Uint16 requirementSkill;
	packet >> requirementSkill;
	requirementSkill_ = requirementSkill;

	// TODO: Find out what this actually is, for now just skipping over it.
	sf::Uint16 unknown;
	packet >> unknown;

	sf::Uint16 useCount;
	packet >> useCount;
	useCount_ = useCount;

	sf::Uint16 maximumUseCount;
	packet >> maximumUseCount;
	maximumUseCount_ = maximumUseCount;
}