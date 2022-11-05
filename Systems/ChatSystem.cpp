
#include "Systems/ChatSystem.h"

#include "Components/ChatComponent.h"

using namespace artemis;

const float ChatSystem::ChatBubbleTimeSpan = 3.0f;

ChatSystem::ChatSystem()
{
	addComponentType<ChatComponent>();
}

void ChatSystem::initialize()
{
	chatMapper.init(*world);
}

void ChatSystem::processEntity(Entity& e)
{
	ChatComponent* chatComponent = chatMapper.get(e);
	if (!chatComponent->GetChat().empty())
	{
		chatComponent->SetTime(chatComponent->GetTime() + world->getDelta());
		if (chatComponent->GetTime() >= ChatBubbleTimeSpan + (chatComponent->GetChat().length()))
		{
			chatComponent->SetChat("");
			chatComponent->SetTime(0);
		}
		else if (chatComponent->GetTime() >= (ChatBubbleTimeSpan + (chatComponent->GetChat().length())) / 2.0f)
		{
			chatComponent->SetColour(CEGUI::colour(192 / 255.f, 192 / 255.f, 192 / 255.f));
		}
		else
		{
			chatComponent->SetColour(CEGUI::colour(255 / 255.f, 255 / 255.f, 255 / 255.f));
		}
	}
}