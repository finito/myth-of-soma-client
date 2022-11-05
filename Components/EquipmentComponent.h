#ifndef EQUIPMENT_COMPONENT_H
#define EQUIPMENT_COMPONENT_H

#include "Artemis/Component.h"

namespace ArmatureType
{
	enum Enum
	{
		Body,
		Top,
		HelmetOrHair,
		Boot,
		Pad,
		Shield,
		Weapon,
		Shadow,
		Aura,
		First = Body,
		Last = Aura + 1
	};
}

class EquipmentComponent : public artemis::Component
{
public:
	EquipmentComponent()
	{
		for (int i = ArmatureType::First; i < ArmatureType::Last; ++i) pictureId[i] = 0;
	}

	// 0 = top
	// 1 = helm or hair
	// 2 = boot
	// 3 = pad
	// 4 = shield
	// 5 = weapon
	// 6 = shadow
	// 7 = aura
	unsigned short pictureId[ArmatureType::Last];
};

#endif // EQUIPMENT_COMPONENT_H