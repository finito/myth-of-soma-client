
#include "MagicData.h"
#include "Network/GamePacket.h"

MagicData::MagicData() :
id_(-1),
method_(0),
characterOrAxis_(0),
startTime_(0),
type_(0),
targetMethod_(0),
range_(0),
manaCost_(0),
duration_(0),
damage_(0),
requirementSkill_(0),
requirementIntelligence_(0),
requirementLevel_(0)
{
}

void MagicData::HandleReceive(Network::GamePacket& packet)
{
	sf::Int16 id;
	packet >> id;
	id_ = id;

	sf::Uint8 method;
	packet >> method;
	method_ = method;

	sf::Uint8 characterOrAxis;
	packet >> characterOrAxis;
	characterOrAxis_ = characterOrAxis;

	sf::Uint16 startTime;
	packet >> startTime;
	startTime_ = startTime;

	sf::Uint8 type;
	packet >> type;
	type_ = type;

	sf::Uint8 targetMethod;
	packet >> targetMethod;
	targetMethod_ = targetMethod;

	packet >> name_;

	packet >> description_;

	sf::Uint16 range;
	packet >> range;
	range_ = range;

	sf::Uint16 manaCost;
	packet >> manaCost;
	manaCost_ = manaCost;

	sf::Uint32 duration;
	packet >> duration;
	duration_ = duration;

	sf::Int16 damage;
	packet >> damage;
	damage_ = damage;

	sf::Uint16 requirementSkill;
	packet >> requirementSkill;
	requirementSkill_ = requirementSkill;

	sf::Uint16 requirementIntelligence;
	packet >> requirementIntelligence;
	requirementIntelligence_ = requirementIntelligence;

	sf::Uint16 requirementLevel;
	packet >> requirementLevel;
	requirementLevel_ = requirementLevel;

	// TODO: Find out what this actually is, for now just skipping over it.
	sf::Uint16 unknown;
	packet >> unknown;
}