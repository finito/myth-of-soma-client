#ifndef SPATIALS_EFFECT_H
#define SPATIALS_EFFECT_H

#include "Spatial.h"
#include <boost/shared_ptr.hpp>

namespace Framework
{
	class TextureAtlasResource;
}

class AnimationComponent;
class EffectComponent;

namespace Spatials
{

class Effect : public Spatial
{
public:
	Effect(artemis::World& world, artemis::Entity& entity);

	virtual void Initialize();
	virtual void Render(Framework::SpriteBatch& spriteBatch);

private:
	AnimationComponent* animationComponent_;
	EffectComponent* effectComponent_;
	boost::shared_ptr<Framework::TextureAtlasResource> textureAtlas_;
};

}; // namespace Spatials

#endif // SPATIALS_EFFECT_H