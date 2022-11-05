
#include "GUI/Common/Slot.h"

#include "Components/ItemAttributeComponent.h"

namespace GUI
{

using namespace CEGUI;

const String Slot::EventNamespace("Slot");

Slot::Slot(const String& type, const String& name) :
Window(type, name)
{
}

Slot::~Slot()
{
}

void Slot::SetContents(const ItemAttributeComponent& item)
{
	Window* quantityText = window_.getChild(window_.getName() + "/Quantity");
	window_.setUserData(&const_cast<ItemAttributeComponent&>(item));

	if (!item.Empty())
	{
		const unsigned int iconGroup = ((item.GetPictureId() - 1) / 81) + 1;
		const unsigned int iconIndex = ((item.GetPictureId() - 1) % 81) + 1;

		const std::string imagePropertyString =
			boost::str(boost::format("set:itemicons%1% image:%2%") % iconGroup % iconIndex);

		window_.setProperty("Image", imagePropertyString);

		switch (item.GetType())
		{
		case ItemType::Potion:
		case ItemType::Food:
		case ItemType::Material:
		case ItemType::Token:
		case ItemType::Quest:
			quantityText->setText(PropertyHelper::uintToString(item.GetDura()));
			break;
		default:
			quantityText->setText("");
			break;
		}
	}
	else
	{
		window_.setProperty("Image", "");
		quantityText->setText("");
	}
}

} // namespace GUI