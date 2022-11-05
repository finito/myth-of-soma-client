#ifndef GUI_CHARACTER_FOCUS_H
#define GUI_CHARACTER_FOCUS_H

#include <CEGUI/CEGUI.h>
#include "Artemis/Entity.h"
#include <SFML/System/Vector2.hpp>
#include "GUI/Moral.h"

namespace GUI
{

class CharacterFocus
{
public:
	CharacterFocus();
	~CharacterFocus();

	void Add(unsigned int serverId);
	void Remove(unsigned int serverId);
	void Update(bool focused, bool showHealthBar, const sf::Vector2f& screenPosition, artemis::Entity& e);
	void Show(unsigned int serverId);
	void Hide(unsigned int serverId);

private:
	CEGUI::Window* window_;
	Moral moralValue_;
};

} // namespace GUI

#endif // GUI_CHARACTER_FOCUS_H