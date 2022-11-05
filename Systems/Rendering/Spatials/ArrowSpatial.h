#ifndef SPATIALS_ARROW_H
#define SPATIALS_ARROW_H

#include "Spatial.h"
#include <boost/shared_ptr.hpp>

namespace Framework
{
	class TextureAtlasResource;
}

class AnimationComponent;
class ArrowComponent;

namespace Spatials
{

class Arrow : public Spatial
{
public:
	Arrow(artemis::World& world, artemis::Entity& entity);

	virtual void Initialize();
	virtual void Render(Framework::SpriteBatch& spriteBatch);

private:
	AnimationComponent* animationComponent_;
	ArrowComponent* arrowComponent_;
	boost::shared_ptr<Framework::TextureAtlasResource> textureAtlas_;
};

}; // namespace Spatials

#endif // SPATIALS_ARROW_H