#ifndef SYSTEMS_BLOOD_SYSTEM_H
#define SYSTEMS_BLOOD_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

#include <map>
#include <vector>

class PositionComponent;
class BloodComponent;

class BloodSystem : public artemis::EntityProcessingSystem
{
public:
	BloodSystem();

	virtual void initialize();

	void processEntity(artemis::Entity& e);

	void GetBloodForEntity(artemis::Entity& e, std::vector<int>& bloodList) const;

protected:
	void added(artemis::Entity& e);
	void removed(artemis::Entity& e);

private:
	std::map<long, std::vector<int>> bloodMap_;

	artemis::ComponentMapper<BloodComponent> bloodMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
};

#endif // SYSTEMS_BLOOD_SYSTEM_H