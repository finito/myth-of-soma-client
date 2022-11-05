#ifndef SYSTEMS_MAGIC_SYSTEM_H
#define SYSTEMS_MAGIC_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

#include "Components/EffectComponent.h" // For EffectType
#include "Components/MagicComponent.h"

class PositionComponent;
class AnimationComponent;
class ExpiresComponent;
class AttributeComponent;
class FlyingEffectComponent;

#include "DataTables/MagicTable.h"

class Zone;
class FocusSystem;

class MagicSystem : public artemis::EntityProcessingSystem
{
public:
	MagicSystem(Zone& zone);

	void initialize();

	void processEntity(artemis::Entity& e);

	void CreateEffectEntity(const MagicTable::MagicElement& magicTable, artemis::Entity& parent, unsigned int effectId,
		EffectType::Enum type, unsigned int lifeTime);

	void FlyingEffect(const MagicComponent& magicComponent, artemis::Entity& target);

	artemis::Entity* GetMagicEntity(artemis::Entity& e, unsigned short magicId);

	bool PlayerMagicAttack(const MagicComponent& magicComponent);
	bool PlayerMagicCast(const unsigned short magicId);

protected:
	void added(artemis::Entity& e);
	void removed(artemis::Entity& e);

private:
	bool PlayerNotInValidState(StateType::Enum state, BattleMode::Enum battleMode) const;
	bool PlayerIsCastingMagic(const std::map<unsigned short, artemis::Entity*>& entities);
	sf::Vector2i GetEntityCellPosition(artemis::Entity& e);
	Direction::Enum GetDirectionToFace(int targetServerId, Direction::Enum initialValue);
	bool TargetIsValid(int targetServerId, short method, int targetType, int playerServerId) const;
	int GetTargetServerId(short method, int playerServerId) const;
	void SendMagicCast(int targetServerId, unsigned short magicId, unsigned short direction);
	void SendMagicAttack(int targetServerId, unsigned short magicId, unsigned short direction);
	bool PlayerMagicAttackArrow(const MagicComponent& magicComponent, AttributeComponent& attributeComponent);

private:
	FocusSystem* focusSystem_;
	Zone& zone_;

	artemis::ComponentMapper<MagicComponent> magicMapper;
	artemis::ComponentMapper<EffectComponent> effectMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
	artemis::ComponentMapper<AnimationComponent> animationMapper;
	artemis::ComponentMapper<ExpiresComponent> expiresMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
	artemis::ComponentMapper<FlyingEffectComponent> flyingEffectMapper;
};

#endif // SYSTEMS_MAGIC_SYSTEM_H