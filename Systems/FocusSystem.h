#ifndef SYSTEMS_FOCUS_SYSTEM_H
#define SYSTEMS_FOCUS_SYSTEM_H

#include "Artemis/EntitySystem.h"
#include "Artemis/ComponentMapper.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

class FocusComponent;
class AttributeComponent;
class AreaComponent;
class PositionComponent;
class ChatComponent;

class Zone;

class CameraSystem;
class MouseCursorSystem;

namespace GUI
{
	class CharacterFocus;
	class ItemFocus;
}

class FocusSystem : public artemis::EntitySystem
{
public:
	FocusSystem(Zone& zone, sf::RenderWindow& renderWindow);

	virtual void initialize();

	virtual void processEntities(artemis::ImmutableBag<artemis::Entity*>& entities);

	int GetCharacterFocusServerId() const;

	void SetCharacterAttackServerId(int serverId);
	int GetCharacterAttackServerId() const;

protected:
	void removed(artemis::Entity& e);

private:
	bool checkProcessing();

	void ShowCharacterFocus(artemis::Entity& e, int houseIndex, bool focused, bool showHealthBar) const;
	bool IsPartyMember();

private:
	sf::RenderWindow& renderWindow_;
	Zone& zone_;
	CameraSystem* cameraSystem_;
	MouseCursorSystem* mouseCursorSystem_;
	boost::scoped_ptr<GUI::CharacterFocus> characterFocus_;
	boost::scoped_ptr<GUI::ItemFocus> itemFocus_;
	
	int characterFocusServerId_;
	int characterAttackServerId_;

	AttributeComponent* attributeComponent_;
	PositionComponent* positionComponent_;

	sf::Vector2f viewOffset_;
	sf::Vector2f viewSize_;

	artemis::ComponentMapper<FocusComponent> focusMapper;
	artemis::ComponentMapper<AttributeComponent> attributeMapper;
	artemis::ComponentMapper<AreaComponent> areaMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
	artemis::ComponentMapper<ChatComponent> chatMapper;
};

#endif // SYSTEMS_FOCUS_SYSTEM_H