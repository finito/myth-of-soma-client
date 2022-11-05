#ifndef SPATIAL_COMPONENT_H
#define SPATIAL_COMPONENT_H

#include "Artemis/Component.h"
#include <string>

class SpatialComponent : public artemis::Component
{
public:
	explicit SpatialComponent(const std::string& type) : type(type) {}

	std::string type;
};

#endif // SPATIAL_COMPONENT_H