#ifndef COMPONENTS_MAGIC_COMPONENT_H
#define COMPONENTS_MAGIC_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <vector>

#include "Components/AttributeComponent.h" // For Direction Enum

#include "DataTables/MagicTable.h"

class MagicComponent : public artemis::Component
{
public:
	MagicComponent(artemis::Entity& source, const MagicTable::MagicElement& magicTable);

	MagicTable::MagicElement& GetMagicTable();
	const MagicTable::MagicElement& GetMagicTable() const;

	artemis::Entity& GetSource() const;

	bool IsReady() const;
	void SetReady(bool value);

	Direction::Enum GetDirection() const;
	void SetDirection(Direction::Enum value);
	void SetDirection(unsigned char value);

	std::vector<artemis::Entity*>& GetTargets();
	const std::vector<artemis::Entity*>& GetTargets() const;

	bool HasAttackSent() const;
	void SetAttackSent(bool value);

private:
	unsigned int id_;
	artemis::Entity& source_;
	MagicTable::MagicElement magicTable_;
	bool ready_;
	Direction::Enum direction_;
	std::vector<artemis::Entity*> targets_;
	bool attackSent_;
};

#endif // COMPONENTS_MAGIC_COMPONENT_H