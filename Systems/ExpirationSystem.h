#ifndef SYSTEMS_EXPIRATION_SYSTEM_H
#define SYSTEMS_EXPIRATION_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

class ExpiresComponent;
class EffectComponent;
class AttributeComponent;

class Zone;

class ExpirationSystem : public artemis::EntityProcessingSystem
{
public:
	ExpirationSystem(Zone& zone);

	virtual void initialize();

	virtual void processEntity(artemis::Entity& e);

private:
	Zone& zone_;

	artemis::ComponentMapper<ExpiresComponent> expiresMapper;
	artemis::ComponentMapper<EffectComponent> effectMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
};

#endif // SYSTEMS_EXPIRATION_SYSTEM_H