#ifndef SYSTEMS_ARROW_SYSTEM_H
#define SYSTEMS_ARROW_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

class ArrowComponent;
class PositionComponent;
class AreaComponent;
class AttributeComponent;

class CameraSystem;

class ArrowSystem : public artemis::EntityProcessingSystem
{
public:
	ArrowSystem();

	virtual void initialize();

	virtual void processEntity(artemis::Entity& e);

protected:
	void added(artemis::Entity& e);

private:
	void SetRadArrays();

	int GetDirection(artemis::Entity& e, const double distanceX, const double distanceY);

	bool SetArrowRoute(artemis::Entity& e);

	const static int MAX_DIRECTION = 32;
	const static int MOVE_RATE = 35;

	double xrad[MAX_DIRECTION];
	double yrad[MAX_DIRECTION];
	double xgaprad[MAX_DIRECTION];
	double ygaprad[MAX_DIRECTION];

	CameraSystem* cameraSystem_;

	artemis::ComponentMapper<ArrowComponent> arrowMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
	artemis::ComponentMapper<AreaComponent> areaMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
};

#endif // SYSTEMS_ARROW_SYSTEM_H