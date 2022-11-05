
#include "Systems/Rendering/TileMapRenderSystem.h"
#include "Systems/CameraSystem.h"

#include "Artemis/SystemManager.h"

#include "Zone.h"
#include "TileMap.h"
#include "MapObjectLoader.h"

#include "Framework/Graphics/SpriteBatch.h"
#include "Framework/Utils/PositionConvert.h"

#include "Game.h"

using namespace artemis;
using namespace Framework;

TileMapRenderSystem::TileMapRenderSystem(sf::RenderTarget& renderTarget, Zone& zone) :
renderTarget_(renderTarget),
zone_(zone),
tileMap_(*zone.GetTileMap()),
mapObjectLoader_(*zone.GetMapObjectLoader()),
spriteBatch_(new SpriteBatch(renderTarget))
{

}

TileMapRenderSystem::~TileMapRenderSystem()
{
}

void TileMapRenderSystem::initialize()
{
	cameraSystem_ = dynamic_cast<CameraSystem*>(world->getSystemManager()->getSystem<CameraSystem>());
}

void TileMapRenderSystem::processEntities(artemis::ImmutableBag<artemis::Entity*>& entities)
{
	renderTarget_.setView(cameraSystem_->GetCamera());

	const unsigned int tileWidth  = static_cast<unsigned int>(std::ceil(cameraSystem_->GetWidth()  / tileSize_) + 1);
	const unsigned int tileHeight = static_cast<unsigned int>(std::ceil(cameraSystem_->GetHeight() / tileSize_) + 1);

	const unsigned int tileOffsetX = std::max(1U, static_cast<unsigned int>(std::floor(cameraSystem_->GetX() / tileSize_)));
	const unsigned int tileOffsetY = std::max(1U, static_cast<unsigned int>(std::floor(cameraSystem_->GetY() / tileSize_)));

	sf::Sprite sprite;
	sf::Texture* paletteTexture;

	for (int l = 0; l < 3; ++l)
	{
		spriteBatch_->Begin(true);
		for (unsigned int x = tileOffsetX - 1; x < tileOffsetX + tileWidth; ++x)
		{
			for (unsigned int y = tileOffsetY - 1; y < tileOffsetY + tileHeight; ++y)
			{
				if (tileMap_.GetTileSprite(l, x, y, sprite, paletteTexture))
				{
					if (paletteTexture != nullptr)
					{
						sprite.setPosition(static_cast<float>(x * tileSize_), static_cast<float>(y * tileSize_));
						spriteBatch_->Draw(sprite, paletteTexture, &Game::Instance().paletteShader);
					}
				}
			}
		}
		spriteBatch_->End();
	}

	spriteBatch_->Begin(true);
	unsigned int yStart = std::max(0, static_cast<int>(tileOffsetY - 9));
	unsigned int xStart = std::max(0, static_cast<int>(tileOffsetX - 1 - (tileWidth / 2)));
	for (unsigned int y = yStart; y < tileOffsetY + tileHeight + (tileHeight * 3); ++y)
	{
		for (unsigned int x = xStart; x < tileOffsetX + tileWidth + (tileWidth / 2); ++x)
		{
			mapObjectLoader_.RenderObject(MapObjectType::Shadow, x, y, *spriteBatch_, renderTarget_);
		}
	}
	spriteBatch_->End();

	renderTarget_.setView(renderTarget_.getDefaultView());
}

bool TileMapRenderSystem::checkProcessing()
{
	return true;
}