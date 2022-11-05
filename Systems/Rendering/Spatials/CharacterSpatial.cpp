
#include "Systems/Rendering/Spatials/CharacterSpatial.h"

#include "Framework/Assets/ResourceCache.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Components/AnimationComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/AreaComponent.h"

#include <boost/format.hpp>

#include "Game.h"
#include "DataTables/ItemTable.h"

using namespace artemis;
using namespace Framework;

namespace Spatials
{

Character::Character(World& world, Entity& entity) :
	Spatial(world, entity)
{
}

void Character::Initialize()
{
	Spatial::Initialize();
	animationComponent_ = entity.getComponent<AnimationComponent>();
	equipmentComponent_ = entity.getComponent<EquipmentComponent>();
	attributeComponent_ = entity.getComponent<AttributeComponent>();
	areaComponent_ = entity.getComponent<AreaComponent>();

	armatureCache_.resize(ArmatureType::Aura+1);
	for (int i = 0; i <= ArmatureType::Aura; ++i)
	{
		armatureCache_[i].index = -1;
	}
}

void Character::Render(SpriteBatch& spriteBatch)
{
	sf::Sprite sprite;
	sprite.setPosition(positionComponent_->GetPosition());

	unsigned char topDrawOrder = 0;
	unsigned char padDrawOrder = 0;
	unsigned char weaponType = 0;

	for (int i = ArmatureType::Body; i <= ArmatureType::Shadow; ++i)
	{
		bool devil = false;
		short armatureIndex = -1;
		if (i == ArmatureType::Body)
		{
			armatureIndex = attributeComponent_->GetSkin();
			if (attributeComponent_->GetServerId() < 10000)
			{
				if (attributeComponent_->GetClassType() >= 10 && attributeComponent_->GetClassType() < 1000)
				{
					armatureIndex += 3 * 2; // HUM_SKIN_COUNT for each gender
					if (attributeComponent_->GetGender() == Gender::Female)
					{
						armatureIndex += 2; // DEV_SKIN_COUNT
					}
				}
				else
				{
					if (attributeComponent_->GetGender() == Gender::Female)
					{
						armatureIndex += 3; // HUM_SKIN_COUNT
					}
				}
			}
		}
		else if (i == ArmatureType::Shadow)
		{
			armatureIndex = attributeComponent_->GetServerId() < 10000 ? 1 : attributeComponent_->GetSkin();
			if (attributeComponent_->GetGender() == Gender::Female) armatureIndex += 500;
		}
		else if (equipmentComponent_ != nullptr)
		{
			if (i == ArmatureType::HelmetOrHair && equipmentComponent_->pictureId[i] == 0)
			{
				 armatureIndex = attributeComponent_->GetHair() + 100;
				 if (attributeComponent_->GetClassType() >= 10 && attributeComponent_->GetClassType() < 1000)
				 {
					 devil = true;
				 }
			}
			else if (equipmentComponent_->pictureId[i] != 0)
			{
				const ItemTable::ItemElement* itemElement = Game::Instance().itemTable->Get(equipmentComponent_->pictureId[i]);
				if (itemElement)
				{
					armatureIndex = itemElement->armatureIndex;

					if (i == ArmatureType::Top)
					{
						topDrawOrder = itemElement->drawOrder;
					}
					if (i == ArmatureType::Pad)
					{
						padDrawOrder = itemElement->drawOrder;
					}
					if (i == ArmatureType::Weapon)
					{
						weaponType = itemElement->equipPosition;
					}
					
					devil = itemElement->devil;
				}
			}
			
			if (attributeComponent_->GetGender() == Gender::Female) armatureIndex += 500;
		}

		if (animationState_ != animationComponent_->animationState || armatureCache_[i].index != armatureIndex)
		{
			armatureCache_[i].index = armatureIndex;
			if (armatureCache_[i].index == -1) continue;

			if (i == ArmatureType::Body)
			{
				armatureCache_[ArmatureType::Body].textureAtlas = Framework::textureAtlasCache.Get(
					boost::str(boost::format("man/man%1$03i%2$02i.dat") % armatureIndex % animationComponent_->animationState));
			}
			else
			{
				if (devil)
				{
					armatureCache_[i].textureAtlas = Framework::textureAtlasCache.Get(boost::str(
						boost::format("darmature/a%1$01i%2$03i%3$02i.dat") % (i - 1) % armatureIndex % animationComponent_->animationState));
				}
				else
				{
					armatureCache_[i].textureAtlas = Framework::textureAtlasCache.Get(boost::str(
						boost::format("armature/a%1$01i%2$03i%3$02i.dat") % (i - 1) % armatureIndex % animationComponent_->animationState));
				}
			}
		}
	}
	animationState_ = animationComponent_->animationState;

	int drawOrder[8];
	drawOrder[0] = ArmatureType::Shadow;
	switch (attributeComponent_->GetDirection())
	{
	case Direction::Down:
		drawOrder[1] = ArmatureType::Body;
		drawOrder[2] = ArmatureType::Pad;
		drawOrder[3] = ArmatureType::Boot;
		drawOrder[4] = ArmatureType::Top;
		drawOrder[5] = ArmatureType::HelmetOrHair;
		drawOrder[6] = ArmatureType::Shield;
		drawOrder[7] = ArmatureType::Weapon;
		if (padDrawOrder == 1 && topDrawOrder >= 1)
		{
			drawOrder[2] = ArmatureType::Boot;
			drawOrder[3] = ArmatureType::Top;
			drawOrder[4] = ArmatureType::Pad;
			drawOrder[5] = -1;
		}
		else
		{
			if (padDrawOrder == 1)
			{
				drawOrder[2] = ArmatureType::Top;
				drawOrder[4] = ArmatureType::Pad;
			}

			if (topDrawOrder >= 2)
			{
				drawOrder[5] = -1;
			}
		}
		break;
	case Direction::Up:
		drawOrder[1] = ArmatureType::Shield;
		drawOrder[2] = ArmatureType::Body;
		drawOrder[3] = ArmatureType::Pad;
		drawOrder[4] = ArmatureType::Boot;
		drawOrder[5] = ArmatureType::Top;
		drawOrder[6] = ArmatureType::Weapon;
		drawOrder[7] = ArmatureType::HelmetOrHair;
		if (padDrawOrder == 1 && topDrawOrder >= 1)
		{
			drawOrder[1] = ArmatureType::Body;
			drawOrder[2] = ArmatureType::Boot;
			drawOrder[3] = ArmatureType::Top;
			drawOrder[4] = ArmatureType::Pad;
			drawOrder[5] = ArmatureType::Weapon;
			drawOrder[6] = ArmatureType::Shield;
			drawOrder[7] = -1;
		}
		else
		{
			if (padDrawOrder == 1)
			{
				drawOrder[3] = ArmatureType::Top;
				drawOrder[5] = ArmatureType::Pad;
			}

			if (topDrawOrder >= 2)
			{
				drawOrder[7] = -1;
			}
		}
		break;
	case Direction::DownLeft:
	case Direction::Left:
	case Direction::UpLeft:
		drawOrder[1] = ArmatureType::Body;
		drawOrder[2] = ArmatureType::Weapon;
		drawOrder[3] = ArmatureType::Pad;
		drawOrder[4] = ArmatureType::Boot;
		drawOrder[5] = ArmatureType::Top;
		drawOrder[6] = ArmatureType::HelmetOrHair;
		drawOrder[7] = ArmatureType::Shield;
		if (padDrawOrder == 1 && topDrawOrder >= 1)
		{
			drawOrder[2] = ArmatureType::Boot;
			drawOrder[3] = ArmatureType::Top;
			drawOrder[4] = ArmatureType::Pad;
			drawOrder[5] = ArmatureType::Weapon;
			drawOrder[6] = -1;
		}
		else
		{
			if (padDrawOrder == 1)
			{
				drawOrder[2] = ArmatureType::Top;
				drawOrder[3] = ArmatureType::Boot;
				drawOrder[4] = ArmatureType::Pad;
				drawOrder[5] = ArmatureType::Weapon;
			}

			if (topDrawOrder != 0 && weaponType == WeaponType::Bow)
			{
				drawOrder[2] = ArmatureType::Pad;
				drawOrder[3] = ArmatureType::Weapon;
			}

			if (topDrawOrder >= 2)
			{
				drawOrder[6] = -1;
			}
		}
		break;
	case Direction::DownRight:
	case Direction::Right:
	case Direction::UpRight:
		drawOrder[1] = ArmatureType::Body;
		drawOrder[2] = ArmatureType::Pad;
		drawOrder[3] = ArmatureType::Boot;
		drawOrder[4] = ArmatureType::Top;
		drawOrder[5] = ArmatureType::HelmetOrHair;
		drawOrder[6] = ArmatureType::Weapon;
		drawOrder[7] = ArmatureType::Shield;
		if (padDrawOrder == 1 && topDrawOrder >= 1)
		{
			drawOrder[2] = ArmatureType::Boot;
			drawOrder[3] = ArmatureType::Top;
			drawOrder[4] = ArmatureType::Pad;
			drawOrder[5] = -1;
		}
		else
		{
			if (padDrawOrder == 1)
			{
				drawOrder[2] = ArmatureType::Top;
				drawOrder[4] = ArmatureType::Pad;
			}

			if (topDrawOrder >= 2)
			{
				drawOrder[5] = -1;
			}
		}
		break;
	}

	for (int i = 0; i <= 7; ++i)
	{
		if (drawOrder[i] == -1 || armatureCache_[drawOrder[i]].index == -1) continue;

		if (attributeComponent_->GetBattleMode() == BattleMode::Normal &&
			(drawOrder[i] == ArmatureType::Weapon || drawOrder[i] == ArmatureType::Shield)) continue;

		armatureCache_[drawOrder[i]].textureAtlas->GetRegionAsSprite(animationComponent_->regionIndex, sprite);
		if (drawOrder[i] == ArmatureType::Body)
		{
			areaComponent_->SetArea(sf::IntRect(
				static_cast<int>(sprite.getPosition().x - sprite.getOrigin().x),
				static_cast<int>(sprite.getPosition().y - sprite.getOrigin().y),
				sprite.getTextureRect().width,
				sprite.getTextureRect().height));
			areaComponent_->SetTransparentPixels(
				armatureCache_[drawOrder[i]].textureAtlas->GetRegionTransparentPixels(animationComponent_->regionIndex));
		}
		if (drawOrder[i] != ArmatureType::Shadow)
		{
			sf::Texture* paletteTexture =
				armatureCache_[drawOrder[i]].textureAtlas->GetRegionPaletteTexture(animationComponent_->regionIndex);
			if (paletteTexture != nullptr)
			{
				sf::Shader* shader = &Game::Instance().paletteShader;
				if (attributeComponent_->GetFlashColor().r != 0 &&
					attributeComponent_->GetFlashColor().g != 0 &&
					attributeComponent_->GetFlashColor().b != 0)
				{
					if (attributeComponent_->GetFlashColorTimer().getElapsedTime().asMilliseconds() < 500)
					{
						sprite.setColor(attributeComponent_->GetFlashColor());
						shader = &Game::Instance().paletteOverlayShader;
					}
					else
					{
						attributeComponent_->SetFlashColor(sf::Color::Black);
					}
				}

				spriteBatch.Draw(sprite, paletteTexture, shader);
			}
		}
		else
		{
			spriteBatch.Draw(sprite, nullptr, &Game::Instance().shadowShader);
		}
	}
}

} // namespace Spatials