
#include "Systems/FocusSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/MouseCursorSystem.h"

#include "Components/FocusComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/AreaComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ChatComponent.h"

#include "Artemis/SystemManager.h"

#include "Framework/Utils/PositionConvert.h"

#include "Zone.h"
#include "Game.h"

#include "GUI/ItemFocus.h"
#include "GUI/CharacterFocus.h"

#include <SFML/Window/Mouse.hpp>

using namespace artemis;

FocusSystem::FocusSystem(Zone& zone, sf::RenderWindow& renderWindow) :
zone_(zone),
renderWindow_(renderWindow),
characterFocus_(new GUI::CharacterFocus),
itemFocus_(new GUI::ItemFocus),
characterFocusServerId_(-1),
characterAttackServerId_(-1)
{
	addComponentType<FocusComponent>();
	addComponentType<AreaComponent>();
}

void FocusSystem::initialize()
{
	cameraSystem_ = dynamic_cast<CameraSystem*>(world->getSystemManager()->getSystem<CameraSystem>());
	mouseCursorSystem_ = dynamic_cast<MouseCursorSystem*>(world->getSystemManager()->getSystem<MouseCursorSystem>());

	focusMapper.init(*world);
	attributeMapper.init(*world);
	areaMapper.init(*world);
	positionMapper.init(*world);
	chatMapper.init(*world);
}

void FocusSystem::processEntities(artemis::ImmutableBag<artemis::Entity*>& entities)
{
	// TODO: Check if the sorting is still a requirement.
	//entities.sort(
	//	[this](Entity* a, Entity* b) -> bool
	//{
	//	PositionComponent* positionComponentA = positionMapper.get(*a);
	//	PositionComponent* positionComponentB = positionMapper.get(*b);
	//	return positionComponentA->GetPosition().y > positionComponentB->GetPosition().y;
	//});

	PositionComponent* playerPositionComponent = positionMapper.get(*Game::Instance().myEntity);
	const int houseIndex = zone_.GetHouse(playerPositionComponent->GetCellPosition());

	// NOTE: Resetting these here could have problems if was to put some systems on a different thread
	// but currently they all run on the same thread.
	characterFocusServerId_ = -1;
	mouseCursorSystem_->SetMouseCursor(MouseCursor::Normal);

	for (int i = 0; i < entities.getCount(); ++i)
	{
		Entity& e = *entities.get(i);

		const sf::Vector2i mousePosition(renderWindow_.mapPixelToCoords(
			sf::Mouse::getPosition(renderWindow_),
			cameraSystem_->GetCamera()));

		viewOffset_ = cameraSystem_->GetOffset();
		viewSize_ = cameraSystem_->GetSize();

		FocusComponent* focusComponent = focusMapper.get(e);
		if (focusComponent->GetType() == "item")
		{
			PositionComponent* positionComponent = positionMapper.get(e);
			AreaComponent* areaComponent = areaMapper.get(e);
			const sf::IntRect area = areaComponent->GetArea();
			const int itemHouseIndex = zone_.GetHouse(positionComponent->GetCellPosition());
			if (area.contains(mousePosition) && houseIndex == itemHouseIndex)
			{
				itemFocus_->Update(e);

				const CEGUI::UVector2 textPosition(
					CEGUI::UDim(0, static_cast<float>(mousePosition.x - viewOffset_.x)),
					CEGUI::UDim(0, static_cast<float>(mousePosition.y - 20 - viewOffset_.y)));

				const CEGUI::UVector2 itemPosition(
					CEGUI::UDim(0, static_cast<float>(area.left - viewOffset_.x)),
					CEGUI::UDim(0, static_cast<float>(area.top  - viewOffset_.y)));

				itemFocus_->Show(textPosition, itemPosition);
				mouseCursorSystem_->SetMouseCursor(MouseCursor::Pickup);
			}
		}
		else if (focusComponent->GetType() == "character")
		{
			attributeComponent_ = attributeMapper.get(e);
			positionComponent_ = positionMapper.get(e);

			const int characterHouseIndex = zone_.GetHouse(positionComponent_->GetCellPosition());
			const bool showHealthBar = (attributeComponent_->IsGray() || IsPartyMember());

			if (characterFocusServerId_ == -1 && attributeComponent_->IsAlive())
			{
				AreaComponent* areaComponent = areaMapper.get(e);
				if (areaComponent->GetArea().contains(mousePosition) && !areaComponent->IsTransparentPixel(mousePosition) &&
					houseIndex == characterHouseIndex)
				{
					characterFocusServerId_ = attributeComponent_->GetServerId();
				}
			}

			ChatComponent* chatComponent = chatMapper.get(e);
			if (characterFocusServerId_ == attributeComponent_->GetServerId() || !chatComponent->GetChat().empty() || attributeComponent_->IsGray())
			{
				ShowCharacterFocus(e, houseIndex, true, showHealthBar);
			}
			else
			{
				if (showHealthBar)
				{
					ShowCharacterFocus(e, houseIndex, false, showHealthBar);
				}
				else
				{
					characterFocus_->Hide(attributeComponent_->GetServerId());
				}
			}
		}
	}

	itemFocus_->SetCanPickup(characterFocusServerId_ == -1 ? true : false);

	AttributeComponent* attributeComponent = nullptr;
	if (characterAttackServerId_ != -1)
	{
		if (zone_.GetServerEntity(characterAttackServerId_) != nullptr)
		{
			attributeComponent = attributeMapper.get(*zone_.GetServerEntity(characterAttackServerId_));
		}
	}
	else if (characterFocusServerId_ != -1)
	{
		if (zone_.GetServerEntity(characterFocusServerId_) != nullptr)
		{
			attributeComponent = attributeMapper.get(*zone_.GetServerEntity(characterFocusServerId_));
		}
	}

	if (attributeComponent != nullptr && attributeComponent->IsAlive())
	{
		AttributeComponent* playerAttributeComponent = attributeMapper.get(*Game::Instance().myEntity);
		switch (playerAttributeComponent->GetBattleMode())
		{
		case BattleMode::Battle:
			if (attributeComponent->GetServerId() < 10000)
			{
				if (attributeComponent->IsGray())
				{
					mouseCursorSystem_->SetMouseCursor(MouseCursor::Battle);
				}
			}
			else
			{
				if (attributeComponent->IsAttackable())
				{
					mouseCursorSystem_->SetMouseCursor(MouseCursor::Battle);
				}
			}
			break;
		case BattleMode::PK:
			if (attributeComponent->GetServerId() < 10000)
			{
				mouseCursorSystem_->SetMouseCursor(MouseCursor::PK);
			}
			else
			{
				if (attributeComponent->IsAttackable())
				{
					mouseCursorSystem_->SetMouseCursor(MouseCursor::PK);
				}
			}
			break;
		}
	}
}

bool FocusSystem::checkProcessing()
{
	return true;
}

void FocusSystem::removed(artemis::Entity& e)
{
	FocusComponent* focusComponent = focusMapper.get(e);
	if (focusComponent->GetType() == "character")
	{
		AttributeComponent* attributeComponent = attributeMapper.get(e);
		characterFocus_->Remove(attributeComponent->GetServerId());
	}
	else if (focusComponent->GetType() == "item")
	{
		itemFocus_->Hide();
	}
}

int FocusSystem::GetCharacterFocusServerId() const
{
	return characterFocusServerId_;
}

void FocusSystem::SetCharacterAttackServerId(int serverId)
{
	characterAttackServerId_ = serverId;
}

int FocusSystem::GetCharacterAttackServerId() const
{
	return characterAttackServerId_;
}

void FocusSystem::ShowCharacterFocus(Entity& e, const int houseIndex, const bool focused, const bool showHealthBar) const
{
	const sf::FloatRect area(viewOffset_, viewSize_);
	const int characterHouseIndex = zone_.GetHouse(positionComponent_->GetCellPosition());
	if (area.contains(positionComponent_->GetPosition()) && houseIndex == characterHouseIndex)
	{
		characterFocus_->Add(attributeComponent_->GetServerId());

		const sf::Vector2f screenPosition(positionComponent_->GetPosition() - viewOffset_);
		characterFocus_->Update(focused, showHealthBar, screenPosition, e);

		characterFocus_->Show(attributeComponent_->GetServerId());
	}
	else
	{
		characterFocus_->Hide(attributeComponent_->GetServerId());
	}
}

bool FocusSystem::IsPartyMember()
{
	AttributeComponent* playerAttributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (!attributeComponent_->GetPartyLeaderName().empty() &&
		 attributeComponent_->GetPartyLeaderName().compare(playerAttributeComponent->GetPartyLeaderName()) == 0)
	{
		return true;
	}
	return false;
}