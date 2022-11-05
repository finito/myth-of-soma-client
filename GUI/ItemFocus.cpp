
#include "GUI/ItemFocus.h"

#include "Components/ItemAttributeComponent.h"
#include "Components/PositionComponent.h"

#include "Network/GameProtocol.h"
#include "Network/GamePacket.h"
#include "Network/GameSocket.h"

#include "Framework/Utils/PositionConvert.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;

ItemFocus::ItemFocus() :
canPickup_(true)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("itemfocus.layout");
		window_->hide();

		frame_ = window_->getChild("ItemFocus");
		nameText_ = frame_->getChild("ItemFocus/Name");

		itemArea_ = WindowManager::getSingleton().createWindow("DefaultWindow", "ItemFocus/Area");
		itemArea_->setSize(UVector2(UDim(0, 32), UDim(0, 32)));
		itemArea_->setMouseInputPropagationEnabled(true);
		itemArea_->setAlwaysOnTop(true);
		itemArea_->subscribeEvent(Window::EventMouseLeavesArea, Event::Subscriber(&ItemFocus::HandleMouseLeaveArea, this));
		itemArea_->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&ItemFocus::HandleMouseClick, this));

		System::getSingleton().getGUISheet()->getChild("FocusRoot")->addChildWindow(window_);
		System::getSingleton().getGUISheet()->getChild("FocusRoot")->addChildWindow(itemArea_);
	}
	catch (Exception& e)
	{
		std::cerr << "ItemFocus: Failed to initialize: " << e.what() << std::endl;
	}
}

ItemFocus::~ItemFocus()
{
	window_->destroy();
}

void ItemFocus::Update(artemis::Entity& item)
{
	//TODO: Use ComponentMapper?
	ItemAttributeComponent* itemAttributeComponent = item.getComponent<ItemAttributeComponent>();
	if (itemAttributeComponent == nullptr)
	{
		return;
	}

	std::string text;
	if (itemAttributeComponent->GetName().compare("Money") == 0)
	{
		text = boost::str(boost::format("%1% Barr") % itemAttributeComponent->GetDura());
	}
	else if (itemAttributeComponent->GetType() >= ItemType::Potion)
	{
		text = boost::str(boost::format("%1% Quantity: %2%") % itemAttributeComponent->GetName() % itemAttributeComponent->GetDura());
	}
	else
	{
		text = itemAttributeComponent->GetName();
	}

	nameText_->setText(text);

	const float textHeight = PropertyHelper::stringToFloat(nameText_->getProperty("VertExtent"));
	nameText_->setHeight(UDim(0, textHeight));

	const float textWidth = PropertyHelper::stringToFloat(nameText_->getProperty("HorzExtent"));
	nameText_->setWidth(UDim(0, textWidth));

	frame_->setWidth(UDim(0, textWidth));
	frame_->setHeight(UDim(0, textHeight));

	itemEntity_ = &item;
}

void ItemFocus::Show(const UVector2& textPosition, const UVector2& itemPosition)
{
	itemArea_->setPosition(itemPosition);
	if (System::getSingleton().getWindowContainingMouse() == itemArea_)
	{
		frame_->setPosition(textPosition);
		window_->show();
	}
}

void ItemFocus::Hide()
{
	window_->hide();
}

bool ItemFocus::HandleMouseLeaveArea(const EventArgs& e)
{
	Hide();
	return true;
}

bool ItemFocus::HandleMouseClick(const EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		//TODO: Use ComponentMapper?
		PositionComponent* positionComponent = Game::Instance().myEntity->getComponent<PositionComponent>();
		PositionComponent* itemPositionComponent = itemEntity_->getComponent<PositionComponent>();
		if (positionComponent != nullptr && itemPositionComponent != nullptr)
		{
			if (abs(positionComponent->GetCellPosition().x - itemPositionComponent->GetCellPosition().x) +
				abs(positionComponent->GetCellPosition().y - itemPositionComponent->GetCellPosition().y) <= 2)
			{
				Network::GamePacket packet(Network::GameProtocol::PKT_ITEM_PICKUP);
				packet << static_cast<short>(itemPositionComponent->GetCellPosition().x)
					   << static_cast<short>(itemPositionComponent->GetCellPosition().y);
				Game::Instance().gameSocket->Send(packet);
			}
		}
	}
	return true;
}

void ItemFocus::SetCanPickup(bool canPickup)
{
	itemArea_->setMousePassThroughEnabled(!canPickup);
}

} // namespace GUI