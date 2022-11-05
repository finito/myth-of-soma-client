#ifndef COMPONENTS_EFFECT_COMPONENT_H
#define COMPONENTS_EFFECT_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"

namespace EffectType
{
	enum Enum
	{
		Pre,
		Post,
	};
}

class EffectComponent : public artemis::Component
{
public:
	EffectComponent(artemis::Entity& parent, EffectType::Enum type, const std::string& filename, unsigned int id, bool lockToParent);

	artemis::Entity& GetParent() const;

	EffectType::Enum GetType() const;

	std::string GetFilename() const;

	unsigned int GetId() const;

	bool IsContinueEffect() const;
	void SetIsContinueEffect(bool value);

	bool IsLockedToParent() const;

private:
	artemis::Entity& parent_;
	EffectType::Enum type_;
	std::string filename_;
	unsigned int id_;
	bool isContinueEffect_;
	bool isLockedToParent_;
};

#endif // COMPONENTS_EFFECT_COMPONENT_H