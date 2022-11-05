
#include "GUI/HUDDead.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

HUDDead::HUDDead()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("huddead.layout");
		window_->hide();
		window_->setAlwaysOnTop(true);
		window_->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&HUDDead::HandleWindowUpdated, this));

		reviveButton_ = static_cast<PushButton*>(window_->getChild("HudDead/ReviveButton"));
		reviveButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUDDead::HandleReviveButtonClicked, this));

		warpTownButton_ = static_cast<PushButton*>(window_->getChild("HudDead/WarpTownButton"));
		warpTownButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&HUDDead::HandleWarpTownButtonClicked, this));

		reiveCount_ = window_->getChild("HudDead/ReviveCount");

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "HUDDead: Failed to initialize: " << e.what() << std::endl;
	}
}

HUDDead::~HUDDead()
{
	window_->destroy();
}

void HUDDead::Show()
{
	elapsedTime_ = 0;
	countDown_ = 9;
	reiveCount_->setText(PropertyHelper::intToString(countDown_));
	window_->moveToFront();
	window_->setModalState(true);
	window_->show();
}

void HUDDead::Hide()
{
	window_->setModalState(false);
	window_->hide();
}

bool HUDDead::HandleWindowUpdated(const EventArgs& e)
{
	if (countDown_ > 0)
	{
		elapsedTime_ += static_cast<const UpdateEventArgs&>(e).d_timeSinceLastFrame;
		if (elapsedTime_ >= 3.0f)
		{
			--countDown_;
			reiveCount_->setText(PropertyHelper::intToString(countDown_));
			elapsedTime_ -= 3.0f;
		}

		if (countDown_ == 0)
		{
			SendReviveAtTown();
		}
	}
	return true;
}

bool HUDDead::HandleReviveButtonClicked(const EventArgs& e)
{
	SendReviveHere();
	return true;
}

bool HUDDead::HandleWarpTownButtonClicked(const EventArgs& e)
{
	SendReviveAtTown();
	return true;
}

void HUDDead::SendReviveHere()
{
	GamePacket packet(GameProtocol::PKT_LIFE);
	packet << static_cast<unsigned char>(1);
	Game::Instance().gameSocket->Send(packet);
}

void HUDDead::SendReviveAtTown()
{
	GamePacket packet(GameProtocol::PKT_LIFE);
	packet << static_cast<unsigned char>(2);
	Game::Instance().gameSocket->Send(packet);
}

} // namespace GUI