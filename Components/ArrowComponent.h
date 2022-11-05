#ifndef COMPONENTS_ARROW_COMPONENT_H
#define COMPONENTS_ARROW_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <queue>

class ArrowComponent : public artemis::Component
{
public:
	ArrowComponent(artemis::Entity& parent, artemis::Entity& target, int type, bool hit);

	int GetType() const;
	void SetType(int value);

	float GetTime() const;
	void SetTime(float value);	

	int GetDirection() const;
	void SetDirection(int value);

	artemis::Entity& GetParent() const;

	artemis::Entity& GetTarget() const;

	std::queue<sf::Vector2f>& GetQueue();

	bool IsHit();

private:
	int type_;
	float time_;
	artemis::Entity& parent_;
	artemis::Entity& target_;
	bool hit_;
	std::queue<sf::Vector2f> queue_;
	int direction_;
};

#endif // COMPONENTS_ARROW_COMPONENT_H