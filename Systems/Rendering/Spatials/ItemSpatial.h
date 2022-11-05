#ifndef SPATIALS_ITEM_H
#define SPATIALS_ITEM_H

#include "Spatial.h"
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

class ItemAttributeComponent;
class AreaComponent;

namespace Spatials
{

class Item : public Spatial
{
public:
	Item(artemis::World& world, artemis::Entity& entity);

	void Initialize();
	sf::Vector2i GetCellPosition() const;
	void Render(Framework::SpriteBatch& spriteBatch);

private:
	boost::scoped_ptr<sf::Sprite> sprite_;
};

}; // namespace Spatials

#endif // SPATIALS_ITEM_H