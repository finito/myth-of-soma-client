#ifndef SYSTEMS_CHAT_SYSTEM_H
#define SYSTEMS_CHAT_SYSTEM_H

#include "Artemis/EntityProcessingSystem.h"
#include "Artemis/ComponentMapper.h"

class ChatComponent;

class ChatSystem : public artemis::EntityProcessingSystem
{
public:
	ChatSystem();

	virtual void initialize();

	virtual void processEntity(artemis::Entity& e);

private:
	static const float ChatBubbleTimeSpan;

	artemis::ComponentMapper<ChatComponent> chatMapper;
};

#endif // SYSTEMS_CHAT_SYSTEM_H