#ifndef COMPONENTS_CHAT_COMPONENT_H
#define COMPONENTS_CHAT_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <string>
#include <CEGUI/CEGUIcolour.h>

class ChatComponent : public artemis::Component
{
public:
	ChatComponent();

	float GetTime() const;
	void SetTime(float value);

	const std::string& GetChat() const;
	void SetChat(const std::string& value);

	const CEGUI::colour& GetColour() const;
	void SetColour(const CEGUI::colour& value);

private:
	float time_;
	std::string chat_;
	CEGUI::colour color_;
};

#endif // COMPONENTS_CHAT_COMPONENT_H