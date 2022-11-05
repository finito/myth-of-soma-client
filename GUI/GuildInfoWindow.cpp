
#include "GUI/GuildInfoWindow.h"

#include <iostream>

namespace GUI
{

using namespace CEGUI;

String GuildInfoWindow::EventGuildInfoResult = "EventGuildInfoResult";

GuildInfoWindow::GuildInfoWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("guildinfowindow.layout");
		window_->hide();
		window_->setAlwaysOnTop(true);
		window_->addEvent(EventGuildInfoResult);
		window_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&GuildInfoWindow::HandleKeyPress, this));

		name_ = static_cast<Editbox*>(window_->getChild("GuildInfo/Name"));
		name_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&GuildInfoWindow::HandleKeyPress, this));

		text_ = static_cast<MultiLineEditbox*>(window_->getChild("GuildInfo/Text")); 
		text_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&GuildInfoWindow::HandleKeyPress, this));

		okButton_ = static_cast<PushButton*>(window_->getChild("GuildInfo/OKButton"));
		okButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildInfoWindow::HandleOKButtonClicked, this));

		cancelButton_ = static_cast<PushButton*>(window_->getChild("GuildInfo/CancelButton"));
		cancelButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildInfoWindow::HandleCancelButtonClicked, this));		

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "GuildInfoWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

GuildInfoWindow::~GuildInfoWindow()
{
	window_->destroy();
}

void GuildInfoWindow::Show(const String& name, const String& text, const bool nameReadOnly, Event::Subscriber resultHandler)
{
	name_->setMousePassThroughEnabled(nameReadOnly);
	name_->setReadOnly(nameReadOnly);
	name_->setText(name);
	text_->setText(text);

	window_->subscribeEvent(EventGuildInfoResult, resultHandler);
	window_->moveToFront();
	window_->setModalState(true);
	window_->show();

	name_->isReadOnly() ? text_->activate() : name_->activate();	
}

void GuildInfoWindow::Hide()
{
	window_->setModalState(false);
	window_->hide();
	window_->fireEvent(EventGuildInfoResult, EventArgs());
	window_->removeEvent(EventGuildInfoResult);
}

bool GuildInfoWindow::HandleOKButtonClicked(const EventArgs& e)
{
	result_ = GuildInfoResult::OK;
	Hide();
	return true;
}

bool GuildInfoWindow::HandleCancelButtonClicked(const EventArgs& e)
{
	result_ = GuildInfoResult::Cancel;
	Hide();
	return true;
}

bool GuildInfoWindow::HandleKeyPress(const EventArgs& e)
{
	const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);
	switch (keyArgs.scancode)
	{
	case Key::Tab:
		if (!name_->isReadOnly() && !text_->isReadOnly())
		{
			name_->isActive() ? text_->activate() : name_->activate();
		}
		break;
	case Key::Escape:
		cancelButton_->fireEvent(PushButton::EventClicked, EventArgs());
		break;
	}
	return false;
}

GuildInfoResult::Enum GuildInfoWindow::GetResult() const
{
	return result_;
}

const String GuildInfoWindow::GetName() const
{
	return name_->getText();
}

void GuildInfoWindow::SetName(const String& value)
{
	name_->setText(value);
}

const String GuildInfoWindow::GetText() const
{
	return text_->getText();
}

void GuildInfoWindow::SetText(const String& value)
{
	text_->setText(value);
}

} // namespace GUI