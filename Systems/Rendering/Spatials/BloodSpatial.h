#ifndef SPATIALS_BLOOD_H
#define SPATIALS_BLOOD_H

#include "Spatial.h"
#include <boost/shared_ptr.hpp>

namespace Framework
{
	class TextureAtlasResource;
}

class BloodComponent;

namespace Spatials
{

class Blood : public Spatial
{
public:
	Blood(artemis::World& world, artemis::Entity& entity);

	virtual void Initialize();
	virtual void Render(Framework::SpriteBatch& spriteBatch);

private:
	BloodComponent* bloodComponent_;
	boost::shared_ptr<Framework::TextureAtlasResource> textureAtlas_;
};

}; // namespace Spatials

#endif // SPATIALS_BLOOD_H