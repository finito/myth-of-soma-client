
#include "Systems/Rendering/RenderSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/BloodSystem.h"

#include "Artemis/SystemManager.h"

#include "Components/SpatialComponent.h"
#include "Components/BloodComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/AreaComponent.h"
#include "Components/PositionComponent.h"
#include "Components/EffectComponent.h"

#include "Systems/Rendering/Spatials/CharacterSpatial.h"
#include "Systems/Rendering/Spatials/ItemSpatial.h"
#include "Systems/Rendering/Spatials/BloodSpatial.h"
#include "Systems/Rendering/Spatials/ArrowSpatial.h"
#include "Systems/Rendering/Spatials/EffectSpatial.h"

#include "Framework/Graphics/SpriteBatch.h"
#include "Framework/Utils/PositionConvert.h"

#include "Zone.h"
#include "MapObjectLoader.h"

#include <iterator>
#include <vector>
#include <algorithm>
#include <boost/make_shared.hpp>

#include "Game.h"

using namespace artemis;
using namespace Spatials;
using namespace Framework;

RenderSystem::RenderSystem(Zone& zone, sf::RenderTarget& renderTarget) :
renderTarget_(renderTarget),
zone_(zone),
mapObjectLoader_(*zone.GetMapObjectLoader()),
spriteBatch_(new SpriteBatch(renderTarget))
{
	addComponentType<SpatialComponent>();
	addComponentType<PositionComponent>();
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::initialize()
{
	cameraSystem_ = dynamic_cast<CameraSystem*>(world->getSystemManager()->getSystem<CameraSystem>());
	bloodSystem_ = dynamic_cast<BloodSystem*>(world->getSystemManager()->getSystem<BloodSystem>());

	spatialMapper.init(*world);
	bloodMapper.init(*world);
	attributeMapper.init(*world);
	areaMapper.init(*world);
	positionMapper.init(*world);
	effectMapper.init(*world);
}

void RenderSystem::begin()
{
	renderTarget_.setView(cameraSystem_->GetCamera());
	spriteBatch_->Begin();
}

void RenderSystem::end()
{
	spriteBatch_->End();
	renderTarget_.setView(renderTarget_.getDefaultView());
}

void RenderSystem::processEntities(artemis::ImmutableBag<artemis::Entity*>& entities)
{
	PositionComponent* playerPositionComponent = positionMapper.get(*Game::Instance().myEntity);
	if (playerPositionComponent == nullptr)
	{
		return;
	}

	const unsigned int tileWidth  = static_cast<unsigned int>(std::ceil(cameraSystem_->GetWidth()  / 32) + 1);
	const unsigned int tileHeight = static_cast<unsigned int>(std::ceil(cameraSystem_->GetHeight() / 32) + 1);

	const unsigned int tileOffsetX = std::max(1U, static_cast<unsigned int>(std::floor(cameraSystem_->GetX() / 32)));
	const unsigned int tileOffsetY = std::max(1U, static_cast<unsigned int>(std::floor(cameraSystem_->GetY() / 32)));

	unsigned int yLimitPrevious = std::max(0, static_cast<int>(tileOffsetY - 9));
	unsigned int yLimit;

	sf::Vector2<unsigned int> xRange(tileOffsetX - 1 - (tileWidth / 2), tileOffsetX + tileWidth + (tileWidth / 2));

	houseIndex_ = zone_.GetHouse(playerPositionComponent->GetCellPosition());

	if (houseIndex_ != -1)
	{
		yLimit = tileOffsetY + tileHeight + (tileHeight * 3);
		RenderTiledObjects(yLimit, xRange, yLimitPrevious);
	}

	for (auto it = bloodLayer_.cbegin(); it != bloodLayer_.cend(); ++it)
	{
		BloodComponent* bloodComponent = bloodMapper.get((*it).second->GetEntity());
		if (bloodComponent->IsStain())
		{
			if (houseIndex_ == zone_.GetHouse((*it).second->GetCellPosition()))
			{
				(*it).second->Render(*spriteBatch_.get());
			}
		}
	}

	std::vector<boost::shared_ptr<Spatials::Spatial>> aliveCharacters;

	for (auto it = characterLayer_.cbegin(); it != characterLayer_.cend(); ++it)
	{
		AttributeComponent* attributeComponent = attributeMapper.get((*it).second->GetEntity());
		if (!attributeComponent->IsAlive())
		{
			if (houseIndex_ == zone_.GetHouse((*it).second->GetCellPosition()))
			{
				(*it).second->Render(*spriteBatch_.get());
				RenderCharacterBlood((*it).second->GetEntity());
			}
		}
		else
		{
			aliveCharacters.push_back((*it).second);
		}
	}

	for (auto it = itemLayer_.cbegin(); it != itemLayer_.cend(); ++it)
	{
		if (houseIndex_ == zone_.GetHouse((*it).second->GetCellPosition()))
		{
			(*it).second->Render(*spriteBatch_.get());
		}
	}

	RenderPostEffects();

	std::sort(aliveCharacters.begin(), aliveCharacters.end(),
		[this](const boost::shared_ptr<Spatials::Spatial>& a, const boost::shared_ptr<Spatials::Spatial>& b) -> bool
	{
		return a->GetPosition().y < b->GetPosition().y;
	});

	for (auto it = aliveCharacters.cbegin(); it != aliveCharacters.cend(); ++it)
	{
		if (houseIndex_ == -1)
		{
			yLimit = static_cast<unsigned int>((*it)->GetPosition().y / 32.0f);
			RenderTiledObjects(yLimit, xRange, yLimitPrevious);
		}

		if (houseIndex_ == zone_.GetHouse((*it)->GetCellPosition()))
		{
			(*it)->Render(*spriteBatch_.get());
			RenderCharacterBlood((*it)->GetEntity());
		}
	}

	if (houseIndex_ == -1)
	{
		yLimit = tileOffsetY + tileHeight + (tileHeight * 3);
		RenderTiledObjects(yLimit, xRange, yLimitPrevious);
	}

	RenderArrows();

	RenderPreEffects();
}

void RenderSystem::added(Entity& e)
{
	boost::shared_ptr<Spatial> spatial = GetSpatial(e);
	if (spatial != nullptr)
	{
		spatial->Initialize();

		SpatialComponent* spatialComponent = spatialMapper.get(e);
		if (spatialComponent->type == "character")
		{
			characterLayer_[e.getId()] = spatial;
		}
		else if (spatialComponent->type == "item")
		{
			itemLayer_[e.getId()] = spatial;
		}
		else if (spatialComponent->type == "effect")
		{
			EffectComponent* effectComponent = effectMapper.get(e);
			if (effectComponent->GetType() == EffectType::Pre)
			{
				preEffectLayer_[e.getId()] = spatial;
			}
			else if (effectComponent->GetType() == EffectType::Post)
			{
				postEffectLayer_[e.getId()] = spatial;
			}
		}
		else if (spatialComponent->type == "blood")
		{
			bloodLayer_[e.getId()] = spatial;
		}
		else if (spatialComponent->type == "arrow")
		{
			arrowLayer_[e.getId()] = spatial;
		}
	}
}

void RenderSystem::removed(Entity& e)
{
	SpatialComponent* spatialComponent = spatialMapper.get(e);
	if (spatialComponent->type == "character")
	{
		auto it = characterLayer_.find(e.getId());
		if (it != characterLayer_.end())
		{
			characterLayer_.erase(e.getId());
		}
	}
	else if (spatialComponent->type == "item")
	{
		auto it = itemLayer_.find(e.getId());
		if (it != itemLayer_.end())
		{
			itemLayer_.erase(e.getId());
		}
	}
	else if (spatialComponent->type == "effect")
	{
		auto it = preEffectLayer_.find(e.getId());
		if (it != preEffectLayer_.end())
		{
			preEffectLayer_.erase(e.getId());
		}

		auto it2 = postEffectLayer_.find(e.getId());
		if (it2 != postEffectLayer_.end())
		{
			postEffectLayer_.erase(e.getId());
		}
	}
	else if (spatialComponent->type == "blood")
	{
		auto it = bloodLayer_.find(e.getId());
		if (it != bloodLayer_.end())
		{
			bloodLayer_.erase(e.getId());
		}
	}
	else if (spatialComponent->type == "arrow")
	{
		auto it = arrowLayer_.find(e.getId());
		if (it != arrowLayer_.end())
		{
			arrowLayer_.erase(e.getId());
		}
	}
}

bool RenderSystem::checkProcessing()
{
	return true;
}

boost::shared_ptr<Spatial> RenderSystem::GetSpatial(Entity& e)
{
	SpatialComponent* spatialComponent = spatialMapper.get(e);
	if (spatialComponent->type == "character")
	{
		return boost::make_shared<Character>(*world, e);
	}
	else if (spatialComponent->type == "item")
	{
		return boost::make_shared<Item>(*world, e);
	}
	else if (spatialComponent->type == "blood")
	{
		return boost::make_shared<Blood>(*world, e);
	}
	else if (spatialComponent->type == "arrow")
	{
		return boost::make_shared<Arrow>(*world, e);
	}
	else if (spatialComponent->type == "effect")
	{
		return boost::make_shared<Effect>(*world, e);
	}
	else
	{
		assert(0 && "Spatial type is invalid for this entity.");
	}
	return boost::shared_ptr<Spatial>();
}

void RenderSystem::RenderTiledObjects(const unsigned int yLimit, const sf::Vector2<unsigned int>& xRange, unsigned int& yLimitPrevious)
{
	for (unsigned int y = yLimitPrevious; y < yLimit; ++y)
	{
		for (unsigned int x = xRange.x; x < xRange.y; ++x)
		{
			mapObjectLoader_.RenderObject(MapObjectType::Normal, x, y, *spriteBatch_, renderTarget_);
			mapObjectLoader_.RenderObject(MapObjectType::HouseOut, x, y, *spriteBatch_, renderTarget_);
		}
	}
	yLimitPrevious = yLimit;
}

void RenderSystem::RenderCharacterBlood(Entity& e)
{
	std::vector<int> bloodList;
	bloodSystem_->GetBloodForEntity(e, bloodList);
	for (auto it = bloodList.begin(); it != bloodList.end(); ++it)
	{
		auto itSpatial = bloodLayer_.find((*it));
		if (itSpatial != bloodLayer_.end())
		{
			(*itSpatial).second->Render(*spriteBatch_);
		}
	}
}

void RenderSystem::RenderPreEffects()
{
	for (auto it = preEffectLayer_.cbegin(); it != preEffectLayer_.cend(); ++it)
	{
		if (houseIndex_ == zone_.GetHouse((*it).second->GetCellPosition()))
		{
			(*it).second->Render(*spriteBatch_);
		}
	}
}

void RenderSystem::RenderPostEffects()
{
	for (auto it = postEffectLayer_.cbegin(); it != postEffectLayer_.cend(); ++it)
	{
		if (houseIndex_ == zone_.GetHouse((*it).second->GetCellPosition()))
		{
			(*it).second->Render(*spriteBatch_);
		}
	}
}

void RenderSystem::RenderArrows()
{
	for (auto it = arrowLayer_.cbegin(); it != arrowLayer_.cend(); ++it)
	{
		if (houseIndex_ == zone_.GetHouse((*it).second->GetCellPosition()))
		{
			(*it).second->Render(*spriteBatch_);
		}
	}
}