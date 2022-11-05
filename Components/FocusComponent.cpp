
#include "Components/FocusComponent.h"
using namespace artemis;

FocusComponent::FocusComponent(const std::string& type) :
type_(type)
{
}

std::string FocusComponent::GetType() const
{
	return type_;
}