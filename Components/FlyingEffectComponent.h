#ifndef COMPONENTS_FLYING_EFFECT_COMPONENT_H
#define COMPONENTS_FLYING_EFFECT_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"

class FlyingEffectComponent : public artemis::Component
{
public:
	FlyingEffectComponent(artemis::Entity& source, artemis::Entity& target, const std::string& filename);

	artemis::Entity& GetSource() const;

	artemis::Entity& GetTarget() const;

	std::string GetFilename() const;

	float GetTime() const;
	void SetTime(float value);

	std::queue<sf::Vector2f>& GetQueue();

	unsigned int GetHitPreEffect() const;
	void SetHitPreEffect(unsigned int value);

	unsigned int GetHitPostEffect() const;
	void SetHitPostEffect(unsigned int value);

	unsigned int GetHitContinueTime() const;
	void SetHitContinueTime(unsigned int value);

private:
	artemis::Entity& source_;
	artemis::Entity& target_;
	std::string filename_;
	float time_;
	std::queue<sf::Vector2f> queue_;
	unsigned int hitPreEffect_;
	unsigned int hitPostEffect_;
	unsigned int hitContinueTime_;
};

#endif // COMPONENTS_FLYING_EFFECT_COMPONENT_H