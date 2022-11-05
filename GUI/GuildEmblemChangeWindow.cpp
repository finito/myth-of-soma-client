
#include "GUI/GuildEmblemChangeWindow.h"

#include <iostream>

#include "Framework/GUI.h"

#include "Game.h"

namespace GUI
{

using namespace CEGUI;

String GuildEmblemChangeWindow::EventGuildEmblemChangeResult = "EventGuildInfoResult";

GuildEmblemChangeWindow::GuildEmblemChangeWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("guildemblemchangewindow.layout");
		window_->hide();
		window_->setAlwaysOnTop(true);
		window_->addEvent(EventGuildEmblemChangeResult);
		window_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&GuildEmblemChangeWindow::HandleKeyPress, this));

		guildEmblem_ = window_->getChild("GuildEmblemChange/Emblem");

		message_ = window_->getChild("GuildEmblemChange/Message");

		okButton_ = static_cast<PushButton*>(window_->getChild("GuildEmblemChange/OKButton"));
		okButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildEmblemChangeWindow::HandleOKButtonClicked, this));

		cancelButton_ = static_cast<PushButton*>(window_->getChild("GuildEmblemChange/CancelButton"));
		cancelButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildEmblemChangeWindow::HandleCancelButtonClicked, this));		

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "GuildEmblemChangeWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

GuildEmblemChangeWindow::~GuildEmblemChangeWindow()
{
	window_->destroy();
}

bool GuildEmblemChangeWindow::Show(const unsigned int cost, Event::Subscriber resultHandler)
{
	try
	{
		ImagesetManager::getSingleton().destroy("guildEmblem");

		Texture* texture = &Game::Instance().gui->CreateTexture();
		texture->loadFromFile("emblems/emblem.png", "");

		Imageset* imageset = &ImagesetManager::getSingleton().create("guildemblem", *texture);
		imageset->defineImage("Emblem", CEGUI::Rect(0, 0, 16, 14), CEGUI::Point(0, 0));

		guildEmblem_->setProperty("Image", PropertyHelper::imageToString(&imageset->getImage("Emblem")));
	}
	catch (InvalidRequestException)
	{
		return false;
	}
	catch (RendererException)
	{
		return false;
	}

	message_->setText("To complete the operation, " + PropertyHelper::uintToString(cost) +
		" Barr will be needed.\n Are you sure you want this picture as mark for your Guild?");

	window_->subscribeEvent(EventGuildEmblemChangeResult, resultHandler);
	window_->moveToFront();
	window_->setModalState(true);
	window_->show();
	return true;
}

void GuildEmblemChangeWindow::Hide()
{
	window_->setModalState(false);
	window_->hide();
	window_->fireEvent(EventGuildEmblemChangeResult, EventArgs());
	window_->removeEvent(EventGuildEmblemChangeResult);
}

bool GuildEmblemChangeWindow::HandleOKButtonClicked(const EventArgs& e)
{
	result_ = GuildEmblemChangeResult::OK;
	Hide();
	return true;
}

bool GuildEmblemChangeWindow::HandleCancelButtonClicked(const EventArgs& e)
{
	result_ = GuildEmblemChangeResult::Cancel;
	Hide();
	return true;
}

bool GuildEmblemChangeWindow::HandleKeyPress(const EventArgs& e)
{
	const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);
	switch (keyArgs.scancode)
	{
	case Key::Return:
		okButton_->fireEvent(PushButton::EventClicked, EventArgs());
		break;
	case Key::Escape:
		cancelButton_->fireEvent(PushButton::EventClicked, EventArgs());
		break;
	}
	return false;
}

GuildEmblemChangeResult::Enum GuildEmblemChangeWindow::GetResult() const
{
	return result_;
}

} // namespace GUI