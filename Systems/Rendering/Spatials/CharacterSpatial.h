#ifndef SPATIALS_CHARACTER_H
#define SPATIALS_CHARACTER_H

#include "Spatial.h"
#include <SFML/System/Vector2.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace Framework
{
	class TextureAtlasResource;
}

class AnimationComponent;
class EquipmentComponent;
class AttributeComponent;
class AreaComponent;

namespace Spatials
{
class Character : public Spatial
{
public:
	Character(artemis::World& world, artemis::Entity& entity);

	virtual void Initialize();
	virtual void Render(Framework::SpriteBatch& spriteBatch);

private:
	struct ArmatureCacheData
	{
		boost::shared_ptr<Framework::TextureAtlasResource> textureAtlas;
		int index;
	};
	unsigned int animationState_;

	boost::shared_ptr<Framework::TextureAtlasResource> textureAtlas_;
	boost::shared_ptr<Framework::TextureAtlasResource> shadowTextureAtlas_;
	std::vector<ArmatureCacheData> armatureCache_;
	std::vector<ArmatureCacheData> activeArmatureCache_;
	AnimationComponent* animationComponent_;
	EquipmentComponent* equipmentComponent_;
	AttributeComponent* attributeComponent_;
	AreaComponent* areaComponent_;
};

}; // namespace Spatials

#endif // SPATIALS_CHARACTER_H