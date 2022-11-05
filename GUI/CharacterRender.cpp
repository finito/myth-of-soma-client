
#include "CharacterRender.h"

#include "Framework/Assets/ResourceCache.h"

#include "DataTables/ItemTable.h"

#include "Game.h"

#include <boost/format.hpp>

using namespace CEGUI;
using namespace Framework;

CharacterRender::CharacterRender(String imagesetName, String imageName) :
frame_(0),
elapsedTime_(0.125f)
{
	renderTexture_.create(256, 256);
	renderTexture_.clear(sf::Color(0, 0, 0, 0));

	imagesetTexture_ = &System::getSingleton().getRenderer()->createTexture();
	imagesetTexture_->loadFromMemory(renderTexture_.getTexture().copyToImage().getPixelsPtr(), Size(256.f, 256.f), Texture::PF_RGBA);

	imageset_ = &ImagesetManager::getSingleton().create(imagesetName, *imagesetTexture_);
	imageset_->defineImage(imageName, Rect(0, 0, 256.f, 256.f), Point(0, 0) );

	sprite_.setPosition(27.f, 95.f);

	hair_ = 0;
	skin_ = 0;
	gender_ = 0;
	devil_ = false;

	for (int i = 0; i < 8; ++i)
	{
		equipmentPictureId_[i] = 0;
	}
}

CharacterRender::~CharacterRender()
{
	System::getSingleton().getRenderer()->destroyTexture(*imagesetTexture_);
	ImagesetManager::getSingleton().destroy(*imageset_);
}

void CharacterRender::Update(Window& window, float deltaTime)
{
	elapsedTime_ += deltaTime;
	if (elapsedTime_ >= 0.125f)
	{
		boost::shared_ptr<Framework::AnimationResource>& animation = Framework::animationCache.Get("inv/inv.ani");
		const unsigned int regionIndex = animation->GetKeyFrameByFrameNumber(0, 0, frame_++ % animation->GetKeyFrameCount(0));

		renderTexture_.pushGLStates();
		renderTexture_.clear(sf::Color(0, 0, 0, 0));

		std::string invFilename[8];
		unsigned char topDrawOrder = 0;
		unsigned char padDrawOrder = 0;
		for (int i = 0; i < 8; ++i)
		{
			bool devil = devil_;
			short index = -1;
			if (i == Body)
			{
				index = skin_;
			}
			else if (i == HelmetOrHair && equipmentPictureId_[i] == 0)
			{
				index = hair_ + 100;
			}
			else if (equipmentPictureId_[i] != 0)
			{
				const ItemTable::ItemElement* itemElement = Game::Instance().itemTable->Get(equipmentPictureId_[i]);
				if (itemElement)
				{
					index = itemElement->armatureIndex;
					if (i == Top)
					{
						topDrawOrder = itemElement->drawOrder;
					}
					if (i == Pad)
					{
						padDrawOrder = itemElement->drawOrder;
					}
					devil = itemElement->devil;
				}
			}

			if (index != -1)
			{
				invFilename[i] = boost::str(boost::format("%1%/inv%2$02i%3$03i.dat")
					% (devil ? "dinv" : "inv") % (gender_ == 0 ? i : i + 10) % index);
			}
		}

		int drawOrder[7];
		drawOrder[0] = Body;
		drawOrder[1] = Pad;
		drawOrder[2] = Boot;
		drawOrder[3] = Top;
		drawOrder[4] = HelmetOrHair;
		drawOrder[5] = Shield;
		drawOrder[6] = Weapon;
		if (padDrawOrder == 1 && topDrawOrder >= 1)
		{
			drawOrder[1] = Boot;
			drawOrder[2] = Top;
			drawOrder[3] = Pad;
			drawOrder[4] = -1;
		}
		else
		{
			if (padDrawOrder == 1)
			{
				drawOrder[1] = Top;
				drawOrder[3] = Pad;
			}

			if (topDrawOrder >= 2)
			{
				drawOrder[4] = -1;
			}
		}

		for (int i = 0; i < 7; ++i)
		{
			if (drawOrder[i] != -1 && !invFilename[drawOrder[i]].empty())
			{
				boost::shared_ptr<TextureAtlasResource> textureAtlas = Framework::textureAtlasCache.Get(invFilename[drawOrder[i]]);
				if (textureAtlas->GetRegionAsSprite(regionIndex, sprite_))
				{
					sf::Texture* paletteTexture = textureAtlas->GetRegionPaletteTexture(regionIndex);
					if (paletteTexture != nullptr)
					{
						sf::RenderStates states;
						states.shader = &Game::Instance().paletteShader;
						if (Game::Instance().lastPaletteTexture != paletteTexture)
						{
							Game::Instance().paletteShader.setParameter("paletteTexture", *paletteTexture);
							Game::Instance().lastPaletteTexture = paletteTexture;
						}
						renderTexture_.draw(sprite_, states);
					}
				}
			}
		}
		renderTexture_.display();
		renderTexture_.popGLStates();

		imagesetTexture_->loadFromMemory(renderTexture_.getTexture().copyToImage().getPixelsPtr(), Size(256.f, 256.f), Texture::PF_RGBA);
		window.invalidate(false);

		elapsedTime_ -= 0.125f;
	}
}

void CharacterRender::SetBody(const unsigned short gender, const unsigned short skin, unsigned short hair, const bool devil)
{
	gender_ = gender;
	skin_ = skin;
	hair_ = hair;
	devil_ = devil;
}

void CharacterRender::SetEquipment(const unsigned char type, const unsigned short pictureId)
{
	if (type < 8)
	{
		equipmentPictureId_[type] = pictureId;
	}
}