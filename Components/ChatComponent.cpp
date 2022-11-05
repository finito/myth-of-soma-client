
#include "Components/ChatComponent.h"

using namespace artemis;

ChatComponent::ChatComponent() :
	time_(0), color_(CEGUI::colour(255 / 255.f, 255 / 255.f, 255 / 255.f))
{
}

float ChatComponent::GetTime() const
{
	return time_;
}

void ChatComponent::SetTime(float value)
{
	time_ = value;
}

const std::string& ChatComponent::GetChat() const
{
	return chat_;
}

void ChatComponent::SetChat(const std::string& value)
{
	chat_ = value;
}

const CEGUI::colour& ChatComponent::GetColour() const
{
	return color_;
}

void ChatComponent::SetColour(const CEGUI::colour& value)
{
	color_ = value;
}