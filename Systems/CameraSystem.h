#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "Artemis/EntitySystem.h"
#include "Artemis/ComponentMapper.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

class PositionComponent;

class CameraSystem : public artemis::EntitySystem
{
public:
	CameraSystem(sf::RenderTarget& renderTarget);

	virtual void initialize();

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*>& entities);

	float GetWidth() const;
	float GetHeight() const;
	sf::Vector2f GetSize() const;

	float GetX() const;
	float GetY() const;
	sf::Vector2f GetOffset() const;

	sf::Vector2f GetCenter() const;

	const sf::View& GetCamera() const;

private:
	bool checkProcessing();

	void EnsurePlayerEntity();

	void UpdateCenter();

	sf::View camera_;
	artemis::Entity* player_;
	sf::RenderTarget& renderTarget_;

	artemis::ComponentMapper<PositionComponent> positionMapper;
};

#endif // CAMERA_SYSTEM_H