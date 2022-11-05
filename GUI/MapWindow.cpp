
#include "GUI/MapWindow.h"

#include "Components/PositionComponent.h"

#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;

MapWindow::MapWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("mapwindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&MapWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&MapWindow::HandleWindowUpdated, this));

		playerPosition_ = window_->getChild("Map/PlayerPosition");

		zoneNumber_ = 0;

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "MapWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

MapWindow::~MapWindow()
{
	window_->destroy();
}

bool MapWindow::SetImage()
{
	zoneNumber_ = Game::Instance().zoneNumber;
	if (!ImagesetManager::getSingleton().isDefined("mapview" + PropertyHelper::uintToString(Game::Instance().zoneNumber)))
	{
		return false;
	}

	Imageset& imageset = ImagesetManager::getSingleton().get("mapview" + PropertyHelper::uintToString(Game::Instance().zoneNumber));
	const Image& image = imageset.getImage("MapView");
	window_->setProperty("Image", PropertyHelper::imageToString(&image));
	window_->setPosition(UVector2(UDim(0.5, -(image.getWidth() / 2)), UDim(0.5, -((image.getHeight() / 2) + 40.f))));
	window_->setSize(UVector2(UDim(0, image.getWidth()), UDim(0, image.getHeight()))); 
	return true;
}

bool MapWindow::HandleShown(const EventArgs& e)
{
	if (SetImage())
	{
		window_->show();
	}
	return true;
}

bool MapWindow::HandleWindowUpdated(const EventArgs& e)
{
	if (zoneNumber_ != Game::Instance().zoneNumber)
	{
		if (!SetImage())
		{
			window_->hide();
		}
	}

	PositionComponent* positionComponent = Game::Instance().myEntity->getComponent<PositionComponent>();
	if (positionComponent != nullptr)
	{
		float positionX = positionComponent->GetCellPosition().x / static_cast<float>(Game::Instance().zoneCellSize.x);
		positionX *= 100;
		positionX *= window_->getPixelSize().d_width;
		positionX /= 100;
		positionX -= playerPosition_->getPixelSize().d_width / 2;

		float positionY = positionComponent->GetCellPosition().y / static_cast<float>(Game::Instance().zoneCellSize.y);
		positionY *= 100;
		positionY *= window_->getPixelSize().d_height;
		positionY /= 100;
		positionY -= playerPosition_->getPixelSize().d_height / 2;

		playerPosition_->setPosition(UVector2(UDim(0, positionX), UDim(0, positionY)));
	}
	return true;
}

} // namespace GUI