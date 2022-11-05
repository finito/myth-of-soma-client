
#include "GUI/CharacterFocus.h"
#include "GUI/GuildEmblem.h"

#include "Components/AttributeComponent.h"
#include "Components/ChatComponent.h"

#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;

CharacterFocus::CharacterFocus()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("characterfocus.layout");
		window_->getChild("CharacterFocus")->hide();
		window_->moveToBack();

		System::getSingleton().getGUISheet()->getChild("FocusRoot")->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "CharacterFocus: Failed to initialize: " << e.what() << std::endl;
	}
}

CharacterFocus::~CharacterFocus()
{
	window_->destroy();
}

void CharacterFocus::Add(const unsigned int serverId)
{
	const String windowName = "CharacterFocus_" + PropertyHelper::intToString(serverId);
	if (!window_->isChild(windowName))
	{
		Window* characterFocus = window_->getChild("CharacterFocus")->clone(windowName);
		window_->addChildWindow(characterFocus);
	}
}

void CharacterFocus::Remove(const unsigned int serverId)
{
	const String windowName = "CharacterFocus_" + PropertyHelper::intToString(serverId);
	if (window_->isChild(windowName))
	{
		window_->getChild(windowName)->destroy();
	}
}

void CharacterFocus::Update(bool focused, bool showHealthBar, const sf::Vector2f& screenPosition, artemis::Entity& e)
{
	//TODO Use ComponentMapper?
	AttributeComponent* attributeComponent = e.getComponent<AttributeComponent>();
	ChatComponent* chatComponent = e.getComponent<ChatComponent>();
	if (!attributeComponent || !chatComponent)
	{
		return;
	}

	const String windowName = "CharacterFocus_" + PropertyHelper::intToString(attributeComponent->GetServerId());
	if (window_->isChild(windowName))
	{
		Window* characterFocus = window_->getChild(windowName);

		Window* characterNameArea    = characterFocus->getChild(characterFocus->getName() + "/NameArea");
		Window* characterName	     = characterNameArea->getChild("CharacterFocus/Name_clone_" + characterNameArea->getName());
		Window* characterGuildName   = characterFocus->getChild(characterFocus->getName() + "/GuildName");
		Window* characterGuildEmblem = characterNameArea->getChild("CharacterFocus/GuildEmblem_clone_" + characterNameArea->getName());
		Window* characterChat	     = characterFocus->getChild(characterFocus->getName() + "/Chat");
		ProgressBar* characterHealthBar = static_cast<ProgressBar*>(characterFocus->getChild(characterFocus->getName() + "/HealthBar"));

		const Size displaySize = System::getSingleton().getRenderer()->getDisplaySize();

		float maxWidth	  = 140.f;
		float totalWidth  = 0.f;
		float totalHeight = 0.f;
		float nameHeight  = 0.f;
		float offsetX	  = -70.f;
		float offsetY	  = -80.f;

		characterGuildName->setVisible(false);
		characterNameArea->setVisible(false);
		characterName->setVisible(false);
		characterChat->setVisible(false);
		characterHealthBar->setVisible(false);
		characterGuildEmblem->setVisible(false);

		if (focused && !attributeComponent->GetGuildName().empty())
		{
			characterGuildName->setVisible(true);
			characterGuildName->setText(attributeComponent->GetGuildName());

			characterGuildName->setPosition(UVector2(UDim(0, 0), UDim(0, totalHeight)));
			characterGuildName->setWidth(UDim(0, maxWidth));

			const float characterGuildNameHeight = PropertyHelper::stringToFloat(characterGuildName->getProperty("VertExtent"));
			characterGuildName->setHeight(UDim(0, characterGuildNameHeight));

			float characterGuildNameWidth = PropertyHelper::stringToFloat(characterGuildName->getProperty("HorzExtent"));
			characterGuildNameWidth += (maxWidth - characterGuildNameWidth) / 2;
			if (characterGuildNameWidth > totalWidth)
			{
				totalWidth = characterGuildNameWidth;
			}
			totalHeight += characterGuildNameHeight;
		}

		if (focused && !attributeComponent->GetName().empty())
		{
			characterNameArea->setVisible(true);
			characterName->setVisible(true);
			characterName->setText(attributeComponent->GetName());
			
			if (attributeComponent->GetServerId() < 10000)
			{
				moralValue_.SetValue(attributeComponent->GetMoralValue());
				characterName->setProperty("TextColours", PropertyHelper::colourToString(moralValue_.GetColor()));
				if (attributeComponent->IsGray())
				{
					if (attributeComponent->GetGrayMode() == 1 ||
						(attributeComponent->GetGrayTimer().getElapsedTime().asMilliseconds() / 200) % 2 == 0)
					{
						characterName->setProperty("TextColours", PropertyHelper::colourToString(colour(180 / 255.f, 180 / 255.f, 180 /255.f)));
					}
				}
			}

			float xPosition = !attributeComponent->GetGuildName().empty() && attributeComponent->GetGuildPictureId() != 0 ? 8.f : 0;
			characterName->setPosition(UVector2(UDim(0, xPosition), UDim(0, totalHeight)));
			characterName->setWidth(UDim(0, maxWidth));

			const float characterNameHeight = PropertyHelper::stringToFloat(characterName->getProperty("VertExtent"));
			characterName->setHeight(UDim(0, characterNameHeight));

			float characterNameWidth = PropertyHelper::stringToFloat(characterName->getProperty("HorzExtent"));
			if (!attributeComponent->GetGuildName().empty())
			{
				characterGuildEmblem->setProperty("Image", Game::Instance().guildEmblem->GetImageAsString(
					attributeComponent->GetGuildId(), attributeComponent->GetGuildPictureId()));
				characterGuildEmblem->setVisible(true);
				characterGuildEmblem->setPosition(UVector2(UDim(0, xPosition - 16.f + (maxWidth - characterNameWidth) / 2), UDim(0, totalHeight)));
			}
			characterNameWidth += (maxWidth - characterNameWidth) / 2;
			if (characterNameWidth > totalWidth)
			{
				totalWidth = characterNameWidth;
			}
			totalHeight += characterNameHeight;
			nameHeight = characterNameHeight;
		}

		if (focused && !chatComponent->GetChat().empty())
		{
			characterChat->setVisible(true);
			characterChat->setText(chatComponent->GetChat());
			characterChat->setProperty("TextColours", PropertyHelper::colourToString(chatComponent->GetColour()));

			characterChat->setPosition(UVector2(UDim(0, 0), UDim(0, totalHeight)));
			characterChat->setWidth(UDim(0, maxWidth));

			const float characterChatHeight = PropertyHelper::stringToFloat(characterChat->getProperty("VertExtent"));
			characterChat->setHeight(UDim(0, characterChatHeight));

			const float characterChatWidth = PropertyHelper::stringToFloat(characterChat->getProperty("HorzExtent"));
			if (characterChatWidth > totalWidth)
			{
				totalWidth = characterChatWidth;
			}
			if (characterChatWidth < maxWidth)
			{
				characterChat->setPosition(UVector2(UDim(0, (maxWidth - characterChatWidth) / 2), UDim(0, totalHeight)));
			}

			// TODO: Is this still needed?
			if (characterChatHeight > characterChat->getFont()->getLineSpacing())
			{
				totalWidth = maxWidth;
			}

			totalHeight += characterChatHeight;
		}

		if (showHealthBar)
		{
			characterHealthBar->setVisible(true);
			characterHealthBar->setProgress(attributeComponent->GetHealthStatus());
			characterHealthBar->setPosition(UVector2(UDim(0, (maxWidth - characterHealthBar->getPixelSize().d_width) / 2),
				UDim(0, totalHeight)));
			totalHeight+= characterHealthBar->getPixelSize().d_height;
		}

		sf::Vector2f focusPosition(screenPosition.x + offsetX, screenPosition.y + offsetY - totalHeight);
		if (focusPosition.x + (maxWidth - totalWidth) < 0)
		{
			focusPosition.x -= focusPosition.x + (maxWidth - totalWidth);
		}
		else if ((focusPosition.x + totalWidth) > displaySize.d_width)
		{
			focusPosition.x -= (focusPosition.x + totalWidth) - displaySize.d_width;
		}
		if (focusPosition.y < 0)
		{
			focusPosition.y = screenPosition.y + nameHeight;
		}

		characterFocus->setPosition(UVector2(UDim(0, focusPosition.x), UDim(0, focusPosition.y)));
		characterFocus->setWidth(UDim(0, maxWidth));
	}
}

void CharacterFocus::Show(const unsigned int serverId)
{
	const String windowName = "CharacterFocus_" + PropertyHelper::intToString(serverId);
	if (window_->isChild(windowName))
	{
		window_->getChild(windowName)->show();
	}
}

void CharacterFocus::Hide(const unsigned int serverId)
{
	const String windowName = "CharacterFocus_" + PropertyHelper::intToString(serverId);
	if (window_->isChild(windowName))
	{
		window_->getChild(windowName)->hide();
	}
}

} // namespace GUI