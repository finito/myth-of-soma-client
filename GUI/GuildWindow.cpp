
#include "GUI/GuildWindow.h"
#include "GUI/InputBoxWindow.h"
#include "GUI/MessageBoxWindow.h"
#include "GUI/GuildInfoWindow.h"
#include "GUI/GuildEmblem.h"
#include "GuildEmblemChangeWindow.h"

#include "Components/AttributeComponent.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include <iostream>
#include <algorithm>
#include <boost/format.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

GuildWindow::GuildWindow() :
selectedMemberSlot_(nullptr),
processing_(false),
quitting_(false),
guildEmblemChangeWindow_(new GuildEmblemChangeWindow)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("guildwindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&GuildWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&GuildWindow::HandleHidden, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&GuildWindow::HandleScrollMembers, this));
		window_->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&GuildWindow::HandleWindowUpdated, this));

		closeButton_ = static_cast<PushButton*>(window_->getChild("Guild/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildWindow::HandleCloseButtonClicked, this));

		guildEmblemButton_ = static_cast<PushButton*>(window_->getChild("Guild/GuildEmblemButton"));
		guildEmblemButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildWindow::HandleGuildEmblemButtonClicked, this));

		guildEmblem_ = guildEmblemButton_->getChild("Guild/GuildEmblem");

		guildNameButton_ = static_cast<PushButton*>(window_->getChild("Guild/GuildName"));
		guildNameButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&GuildWindow::HandleGuildNameButtonClicked, this));

		memberCountText_ = window_->getChild("Guild/MemberCount");

		stashButton_ = static_cast<PushButton*>(window_->getChild("Guild/StashButton"));
		stashButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildWindow::HandleStashButtonClicked, this));

		dismissButton_ = static_cast<PushButton*>(window_->getChild("Guild/DismissButton"));
		dismissButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildWindow::HandleDismissButtonClicked, this));

		for (int i = 0; i < RANKED_MEMBER_SLOT_COUNT; ++i)
		{
			rankedMemberSlots_[i] = window_->getChild("Guild/RankedMember" + PropertyHelper::intToString(i+1));
			rankedMemberSlots_[i]->setID(i);
			rankedMemberSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&GuildWindow::HandleRankMemberSlotMouseClick, this));
			rankedMemberSlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&GuildWindow::HandleRankedMemberSlotDragDropItemDropped, this));

			rankedMemberTitleTexts_[i] = rankedMemberSlots_[i]->getChild(rankedMemberSlots_[i]->getName() + "/Title");
			rankedMemberNameTexts_[i] = rankedMemberSlots_[i]->getChild(rankedMemberSlots_[i]->getName() + "/Name");

			DragContainer* dragContainer = static_cast<DragContainer*>(
				rankedMemberSlots_[i]->getChild(rankedMemberSlots_[i]->getName() + "/DragContainer"));
			dragContainer->setID(i);
			dragContainer->setDragDropTarget(false);
			dragContainer->setAlpha(0);
			dragContainer->setUserString("type", "rankedMember");
			dragContainer->subscribeEvent(DragContainer::EventDragStarted,
				Event::Subscriber(&GuildWindow::HandleRankMemberSlotDragStarted, this));
			dragContainer->subscribeEvent(DragContainer::EventDragEnded,
				Event::Subscriber(&GuildWindow::HandleRankMemberSlotDragEnded, this));
			dragContainer->subscribeEvent(DragContainer::EventMouseButtonUp,
				Event::Subscriber(&GuildWindow::HandleDragMouseButtonUp, this));


		}

		for (int i = 0; i < MEMBER_SLOT_COUNT; ++i)
		{
			memberSlots_[i] = window_->getChild("Guild/Member" + PropertyHelper::intToString(i+1));
			memberSlots_[i]->setID(i);
			memberSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&GuildWindow::HandleMemberSlotMouseClick, this));
			memberSlots_[i]->subscribeEvent(Window::EventDragDropItemDropped,
				Event::Subscriber(&GuildWindow::HandleRankedMemberSlotDragDropItemDropped, this));

			memberNameTexts_[i] = memberSlots_[i]->getChild(memberSlots_[i]->getName() + "/Name");

			DragContainer* dragContainer = static_cast<DragContainer*>(
				memberSlots_[i]->getChild(memberSlots_[i]->getName() + "/DragContainer"));
			dragContainer->setID(i);
			dragContainer->setDragDropTarget(false);
			dragContainer->setAlpha(0);
			dragContainer->setUserString("type", "member");
			dragContainer->subscribeEvent(DragContainer::EventDragStarted,
				Event::Subscriber(&GuildWindow::HandleMemberSlotDragStarted, this));
			dragContainer->subscribeEvent(DragContainer::EventDragEnded,
				Event::Subscriber(&GuildWindow::HandleMemberSlotDragEnded, this));
			dragContainer->subscribeEvent(DragContainer::EventMouseButtonUp,
				Event::Subscriber(&GuildWindow::HandleDragMouseButtonUp, this));
		}

		memberScrollBar_ = static_cast<Scrollbar*>(window_->getChild("Guild/MemberScrollbar"));
		memberScrollBar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&GuildWindow::HandleScrollPositionChanged, this));

		moneyButton_ = static_cast<PushButton*>(window_->getChild("Guild/MoneyButton"));
		moneyButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildWindow::HandlePutMoney, this));

		guildMoneyText_ = window_->getChild("Guild/GuildMoney");

		guildMoneyButton_ = static_cast<PushButton*>(window_->getChild("Guild/GuildMoneyButton"));
		guildMoneyButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuildWindow::HandleGetMoney, this));

		memberSlotSelection_ = window_->getChild("Guild/MemberSlotSelection");
		window_->removeChildWindow(memberSlotSelection_);

		rankedMemberSlotSelection_ = window_->getChild("Guild/RankedMemberSlotSelection");
		window_->removeChildWindow(rankedMemberSlotSelection_);

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "GuildWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

GuildWindow::~GuildWindow()
{
	memberSlotSelection_->destroy();
	rankedMemberSlotSelection_->destroy();
	window_->destroy();
}

bool GuildWindow::HandleShown(const EventArgs& e)
{
	window_->setModalState(true);
	return true;
}

bool GuildWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool GuildWindow::HandleWindowUpdated(const EventArgs& e)
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent != nullptr)
	{
		guildEmblem_->setProperty("Image", Game::Instance().guildEmblem->GetImageAsString(
			attributeComponent->GetGuildId(), attributeComponent->GetGuildPictureId()));
	}
	return true;
}

void GuildWindow::Update()
{
	assert(Game::Instance().myEntity);
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent == nullptr)
	{
		return;
	}

	const float scrollPosition = memberScrollBar_->getScrollPosition();
	const float stepSize	   = memberScrollBar_->getStepSize();
	const float documentSize   = stepSize * std::ceilf(memberNames_.size() / stepSize);

	memberScrollBar_->setDocumentSize(documentSize);
	memberScrollBar_->setScrollPosition(std::min(scrollPosition, documentSize));

	const unsigned int startPosition = static_cast<const unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	std::sort(memberNames_.begin(), memberNames_.end());

	unsigned int memberCount = memberNames_.size();

	for (int i = 0; i < RANKED_MEMBER_SLOT_COUNT; ++i)
	{
		if (!rankedMemberNameTexts_[i]->getText().empty())
		{
			if (rankedMemberNameTexts_[i]->getText().compare(attributeComponent->GetName()) == 0)
			{
				myIndex_ = i;
			}
			++memberCount;
		}
	}

	for (int i = 0; i < MEMBER_SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < memberNames_.size())
		{
			if (memberNames_[index].compare(attributeComponent->GetName()) == 0)
			{
				myIndex_ = i + RANKED_MEMBER_SLOT_COUNT;
			}
			memberNameTexts_[i]->setText(memberNames_[index]);
		}
		else
		{
			memberNameTexts_[i]->setText("");
		}
	}

	memberCountText_->setText(PropertyHelper::uintToString(memberCount));
}

void GuildWindow::HandleMemberList(GamePacket& packet)
{
	memberNames_.clear();

	if (selectedMemberSlot_ != nullptr)
	{
		selectedMemberSlot_->removeChildWindow(memberSlotSelection_);
		selectedMemberSlot_->removeChildWindow(rankedMemberSlotSelection_);
		selectedMemberSlot_ = nullptr;
	}
	processing_ = false;
	quitting_ = false;

	sf::Uint8 rank;
	packet >> rank;
	rank_ = rank;

	sf::Uint16 memberCount;
	packet >> memberCount;

	sf::Uint16 memberOnlyCount;
	memberOnlyCount = memberCount;

	std::string guildName;
	packet >> guildName;
	guildNameButton_->setText(guildName);

	sf::Uint16 taxRate;
	packet >> taxRate;

	sf::Uint32 guildMoney;
	packet >> guildMoney;
	guildMoney_ = guildMoney;
	guildMoneyText_->setText(PropertyHelper::uintToString(guildMoney_));

	for (int i = 0; i < RANKED_MEMBER_SLOT_COUNT; ++i)
	{
		std::string name;
		packet >> name;
		if (!name.empty())
		{
			++memberCount;
		}
		rankedMemberNameTexts_[i]->setText(name);

		std::string title;
		packet >> title;
		rankedMemberTitleTexts_[i]->setText(title);
	}

	memberNames_.resize(memberOnlyCount);
	for (int i = 0; i < memberOnlyCount; ++i)
	{
		std::string name;
		packet >> name;
		memberNames_[i] = name;
	}

	Update();

	window_->setModalState(true);
	window_->show();
}

bool GuildWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

bool GuildWindow::HandleScrollMembers(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	float scrollAmount = 0;
	if (mouseEventArgs.wheelChange < 0)
	{
		scrollAmount = memberScrollBar_->getStepSize();
	}
	else if (mouseEventArgs.wheelChange > 0)
	{
		scrollAmount = -memberScrollBar_->getStepSize();
	}
	// The data must first be refreshed so that 
	// the slot will contain the correct data.
	// The scroll position change event is taking care of the update.
	memberScrollBar_->setScrollPosition(memberScrollBar_->getScrollPosition() + scrollAmount);
	return true;
}

bool GuildWindow::HandleCloseButtonClicked(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool GuildWindow::HandleStashButtonClicked(const EventArgs& e)
{
	if (!processing_)
	{
		GamePacket packet(GameProtocol::PKT_GUILD_STORAGEOPEN);
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

bool GuildWindow::HandleRankMemberSlotMouseClick(const EventArgs&e)
{
	if (processing_)
	{
		return true;
	}

	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	const unsigned int i = mouseEventArgs.window->getID();
	if (mouseEventArgs.button == RightButton && rank_ == GUILD_RANK_LEADER)
	{
		SelectRankedMemberSlot(i);
		Game::Instance().inputBoxWindow->Show("Position Name", Event::Subscriber(&GuildWindow::HandleRankMemberSlotInputResult, this));
		Game::Instance().inputBoxWindow->SetInputText(rankedMemberTitleTexts_[selectedMemberSlot_->getID()]->getText());
	}
	else if (mouseEventArgs.button == LeftButton)
	{
		if (!IsRankedMemberSlotEmpty(i))
		{
			if (rank_ == GUILD_RANK_LEADER)
			{
				SelectRankedMemberSlot(i);
			}
			else if (rank_ == GUILD_RANK_FIRST || rank_ == GUILD_RANK_SECOND)
			{
				if (i == myIndex_)
				{
					SelectRankedMemberSlot(i);
				}
			}
		}
	}
	return true;
}

bool GuildWindow::HandleRankMemberSlotInputResult(const EventArgs&e)
{
	if (selectedMemberSlot_ == nullptr)
	{
		return true;
	}

	processing_ = true;
	processFromIndex_ = selectedMemberSlot_->getID();
	processString_ = Game::Instance().inputBoxWindow->GetInputText();

	const std::string& positionName = processString_.c_str();
	GamePacket packet(GameProtocol::PKT_GUILD_CHANGE_POS);
	packet << static_cast<unsigned char>(processFromIndex_ + 1) << positionName;
	Game::Instance().gameSocket->Send(packet);

	return true;
}

void GuildWindow::HandlePositionNameChanged(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		rankedMemberTitleTexts_[processFromIndex_]->setText(processString_);
	}
	else
	{
		sf::Uint8 errorCode;
		packet >> errorCode;

		switch (errorCode)
		{
		case GameProtocol::GUILD_NOT_GUILD_USER:
			Game::Instance().messageBoxWindow->Show("You are not one of this Guild's members.");
			break;
		case GameProtocol::GUILD_SMALL_RANK:
			Game::Instance().messageBoxWindow->Show("You do not have the permission.");
			break;
		case GameProtocol::GUILD_INVALID_RANK:
			Game::Instance().messageBoxWindow->Show("You cannot change this title.");
			break;
		case GameProtocol::GUILD_INVALID_GUILD_CALL_NAME:
			Game::Instance().messageBoxWindow->Show("The Title is invalid. Please change it to another one.");
			break;
		case GameProtocol::GUILD_ABSENT_GUILD_NAME:
			Game::Instance().messageBoxWindow->Show("This Guild does not exist.");
			break;
		default:
			Game::Instance().messageBoxWindow->Show("You cannot currently change the title. Try again later.");
			break;
		}
	}
}

bool GuildWindow::HandlePutMoney(const EventArgs& e)
{
	if (!processing_)
	{
		AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
		if (attributeComponent != nullptr)
		{
			Game::Instance().inputBoxWindow->Show("How Much?", Event::Subscriber(&GuildWindow::HandlePutMoneyInputResult, this));
			Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(attributeComponent->GetMoney()));
		}
	}
	return true;
}

bool GuildWindow::HandlePutMoneyInputResult(const EventArgs& e)
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent == nullptr)
	{
		return true;
	}

	const unsigned int money = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (money > 0 && money <= attributeComponent->GetMoney())
	{
		SendPutMoney(money);
	}
	else
	{
		Game::Instance().messageBoxWindow->Show("You do not have enough money.");
	}
	return true;
}

bool GuildWindow::HandleGetMoney(const EventArgs& e)
{
	if (!processing_ && rank_ == GUILD_RANK_LEADER)
	{
		if (guildMoney_ > 0)
		{
			Game::Instance().inputBoxWindow->Show("How Much?", Event::Subscriber(&GuildWindow::HandleGetMoneyInputResult, this));
			Game::Instance().inputBoxWindow->SetInputText(PropertyHelper::uintToString(guildMoney_));
		}
		else
		{
			Game::Instance().messageBoxWindow->Show("The guild does not have any money.");
		}
	}
	return true;
}

bool GuildWindow::HandleGetMoneyInputResult(const EventArgs& e)
{
	const unsigned int money = PropertyHelper::stringToUint(Game::Instance().inputBoxWindow->GetInputText());
	if (money > 0 && money <= guildMoney_)
	{
		SendGetMoney(money);
	}
	else
	{
		Game::Instance().messageBoxWindow->Show("The guild does not have that much money.");
	}
	return true;
}

void GuildWindow::SendPutMoney(unsigned int money)
{
	GamePacket packet(GameProtocol::PKT_GUILD_MONEY);
	packet << GameProtocol::GUILD_MONEY_PUT << money;
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

void GuildWindow::SendGetMoney(unsigned int money)
{
	GamePacket packet(GameProtocol::PKT_GUILD_MONEY);
	packet << GameProtocol::GUILD_MONEY_GET << money;
	Game::Instance().gameSocket->Send(packet);
	processing_ = true;
}

void GuildWindow::HandleMoney(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 type;
	packet >> type;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		sf::Uint32 remainingGuildMoney;
		packet >> remainingGuildMoney;

		AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
		if (attributeComponent != nullptr)
		{
			attributeComponent->SetMoney(attributeComponent->GetMoney() + (guildMoney_ - remainingGuildMoney));
		}
		guildMoney_ = remainingGuildMoney;
		guildMoneyText_->setText(PropertyHelper::uintToString(guildMoney_));

		if (type == GameProtocol::GUILD_MONEY_PUT)
		{
			Game::Instance().messageBoxWindow->Show("You have successfully donated to the guild.");
		}
		else if (type == GameProtocol::GUILD_MONEY_GET)
		{
			Game::Instance().messageBoxWindow->Show("You have successfully withdrawn money from the guild.");
		}
	}
	else
	{
		if (type == GameProtocol::GUILD_MONEY_PUT)
		{
			Game::Instance().messageBoxWindow->Show("You failed to donate money.");
		}
		else if (type == GameProtocol::GUILD_MONEY_GET)
		{
			Game::Instance().messageBoxWindow->Show("You failed to withdraw money from the guild.");
		}
	}
}

bool GuildWindow::HandleMemberSlotMouseClick(const EventArgs& e)
{
	if (processing_)
	{
		return true;
	}

	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton)
	{
		const unsigned int i = mouseEventArgs.window->getID();
		if (!IsMemberSlotEmpty(i))
		{
			if (rank_ == GUILD_RANK_LEADER || rank_ == GUILD_RANK_FIRST)
			{
				SelectMemberSlot(i);
			}
			else if (rank_ == GUILD_RANK_MEMBER)
			{
				if (i + RANKED_MEMBER_SLOT_COUNT == myIndex_)
				{
					SelectMemberSlot(i);
				}
			}
		}
	}
	return true;
}

bool GuildWindow::HandleRankMemberSlotDragStarted(const EventArgs& e)
{
	const WindowEventArgs& windowEventArgs = static_cast<const WindowEventArgs&>(e);

	const unsigned int i = windowEventArgs.window->getID();
	if (processing_ || IsRankedMemberSlotEmpty(i))
	{
		DragContainer* dragContainer = static_cast<DragContainer*>(windowEventArgs.window);
		dragContainer->setDraggingEnabled(false);
	}
	else
	{
		Window* title = windowEventArgs.window->getChild(windowEventArgs.window->getName() + "/Title");
		rankedMemberTitleTexts_[windowEventArgs.window->getID()]->clonePropertiesTo(*title);

		Window* name = windowEventArgs.window->getChild(windowEventArgs.window->getName() + "/Name");
		rankedMemberNameTexts_[windowEventArgs.window->getID()]->clonePropertiesTo(*name);

		SelectRankedMemberSlot(windowEventArgs.window->getID());
	}
	return true;
}

bool GuildWindow::HandleRankMemberSlotDragEnded(const EventArgs& e)
{
	return true;
}

bool GuildWindow::HandleMemberSlotDragStarted(const EventArgs& e)
{
	const WindowEventArgs& windowEventArgs = static_cast<const WindowEventArgs&>(e);

	const unsigned int i = windowEventArgs.window->getID();
	if (processing_ || IsMemberSlotEmpty(i))
	{
		DragContainer* dragContainer = static_cast<DragContainer*>(windowEventArgs.window);
		dragContainer->setDraggingEnabled(false);
	}
	else
	{
		Window* name = windowEventArgs.window->getChild(windowEventArgs.window->getName() + "/Name");
		memberNameTexts_[windowEventArgs.window->getID()]->clonePropertiesTo(*name);

		SelectMemberSlot(windowEventArgs.window->getID());

		windowEventArgs.window->setAlpha(1);
	}
	return true;
}

bool GuildWindow::HandleMemberSlotDragEnded(const EventArgs& e)
{
	const WindowEventArgs& windowEventArgs = static_cast<const WindowEventArgs&>(e);
	windowEventArgs.window->setAlpha(0);
	return true;
}

void GuildWindow::SelectRankedMemberSlot(const unsigned int index)
{
	if (index < RANKED_MEMBER_SLOT_COUNT)
	{
		selectedMemberSlot_ = rankedMemberSlots_[index];
		selectedMemberSlot_->addChildWindow(rankedMemberSlotSelection_);
		if (memberSlotSelection_->getParent() != nullptr)
		{
			memberSlotSelection_->getParent()->removeChildWindow(memberSlotSelection_);
		}
	}
}

void GuildWindow::SelectMemberSlot(const unsigned int index)
{
	if (index < MEMBER_SLOT_COUNT)
	{
		selectedMemberSlot_ = memberSlots_[index];
		selectedMemberSlot_->addChildWindow(memberSlotSelection_);
		if (rankedMemberSlotSelection_->getParent() != nullptr)
		{
			rankedMemberSlotSelection_->getParent()->removeChildWindow(rankedMemberSlotSelection_);
		}
	}
}

bool GuildWindow::IsRankedMemberSlotEmpty(const unsigned int index) const
{
	return rankedMemberNameTexts_[index]->getText().empty();
}

bool GuildWindow::IsMemberSlotEmpty(const unsigned int index) const
{
	return memberNameTexts_[index]->getText().empty();
}

bool GuildWindow::HandleRankedMemberSlotDragDropItemDropped(const EventArgs& e)
{
	const DragDropEventArgs& dragDropEventArgs = static_cast<const DragDropEventArgs&>(e);

	if (dragDropEventArgs.dragDropItem->getUserString("type") == "rankedMember")
	{
		fromIndex_ = dragDropEventArgs.dragDropItem->getID();

		if (rank_ == GUILD_RANK_LEADER)
		{
			if (IsRankedMemberSlot(*dragDropEventArgs.window))
			{
				toIndex_ = dragDropEventArgs.window->getID();
				if (dragDropEventArgs.window->getID() == 0)
				{
					HandleChangeMemberRank(true);
				}
				else
				{
					HandleChangeMemberRank(false);
				}
			}
			else if (IsMemberSlot(*dragDropEventArgs.window))
			{
				toIndex_ = 10;
				HandleChangeMemberRank(false);
			}
		}
	}
	else if (dragDropEventArgs.dragDropItem->getUserString("type") == "member")
	{
		fromIndex_ = dragDropEventArgs.dragDropItem->getID() + RANKED_MEMBER_SLOT_COUNT;

		if (rank_ == GUILD_RANK_LEADER)
		{
			if (IsRankedMemberSlot(*dragDropEventArgs.window))
			{
				toIndex_ = dragDropEventArgs.window->getID();
				if (dragDropEventArgs.window->getID() == 0)
				{
					HandleChangeMemberRank(true);
				}
				else
				{
					HandleChangeMemberRank(false);
				}
			}
		}
	}
	return true;
}

bool GuildWindow::IsRankedMemberSlot(const Window& window) const
{
	String::size_type found = window.getName().find("Guild/RankedMember");
	return found != String::npos;
}

bool GuildWindow::IsMemberSlot(const Window& window) const
{
	String::size_type found = window.getName().find("Guild/Member");
	return found != String::npos;
}

void GuildWindow::HandleChangeMemberRank(bool leader)
{
	if (leader)
	{
		Game::Instance().messageBoxWindow->Show("Do you really want to resign as Guild Master of this guild?",
			MessageBoxMode::OKCancel, Event::Subscriber(&GuildWindow::HandleChangeMemberRankMessageBoxResult, this));
	}
	else
	{
		Game::Instance().messageBoxWindow->Show("Do you really want to change the role of this Guild member?",
			MessageBoxMode::OKCancel, Event::Subscriber(&GuildWindow::HandleChangeMemberRankMessageBoxResult, this));
	}
}

bool GuildWindow::HandleChangeMemberRankMessageBoxResult(const EventArgs& e)
{
	if (Game::Instance().messageBoxWindow->GetResult() != MessageBoxResult::OK)
	{
		return true;
	}

	processing_ = true;
	processFromIndex_ = fromIndex_;
	processToIndex_ = toIndex_;

	std::string memberName;
	if (fromIndex_ < RANKED_MEMBER_SLOT_COUNT)
	{
		memberName = rankedMemberNameTexts_[fromIndex_]->getText().c_str();
	}
	else
	{
		memberName = memberNameTexts_[fromIndex_-RANKED_MEMBER_SLOT_COUNT]->getText().c_str();
		fromIndex_ = RANKED_MEMBER_SLOT_COUNT;
	}

	if (toIndex_ >= RANKED_MEMBER_SLOT_COUNT)
	{
		toIndex_ = RANKED_MEMBER_SLOT_COUNT;
	}

	GamePacket packet(GameProtocol::PKT_GUILD_CHANGE_RANK);
	packet << memberName
		   << static_cast<unsigned char>(fromIndex_ + 1)
		   << static_cast<unsigned char>(toIndex_ + 1);
	Game::Instance().gameSocket->Send(packet);
	return true;
}

void GuildWindow::HandleMemberRankChanged(GamePacket& packet)
{
	processing_ = false;

	selectedMemberSlot_->removeChildWindow(memberSlotSelection_);
	selectedMemberSlot_->removeChildWindow(rankedMemberSlotSelection_);

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		if (quitting_)
		{
			window_->hide();
			return;
		}

		String memberName;
		if (processFromIndex_ < RANKED_MEMBER_SLOT_COUNT)
		{
			memberName = rankedMemberNameTexts_[processFromIndex_]->getText();
			rankedMemberNameTexts_[processFromIndex_]->setText("");
		}
		else
		{
			memberName = memberNameTexts_[processFromIndex_-RANKED_MEMBER_SLOT_COUNT]->getText();
			memberNames_.erase(std::remove(memberNames_.begin(), memberNames_.end(), memberName), memberNames_.end());
		}

		if (processToIndex_ == 0)
		{
			memberNames_.push_back(rankedMemberNameTexts_[processToIndex_]->getText().c_str());
			rank_ = GUILD_RANK_MEMBER;
			rankedMemberNameTexts_[processToIndex_]->setText(memberName);
		}
		else if (processToIndex_ < RANKED_MEMBER_SLOT_COUNT)
		{
			rankedMemberNameTexts_[processToIndex_]->setText(memberName);
		}
		else
		{
			memberNames_.push_back(memberName.c_str());
		}

		Update();
	}
	else
	{
		sf::Uint8 errorCode;
		packet >> errorCode;

		switch (errorCode)
		{
		case GameProtocol::GUILD_NOT_GUILD_USER:
			Game::Instance().messageBoxWindow->Show("You are not one of this Guild's members.");
			break;
		case GameProtocol::GUILD_SMALL_RANK:
			Game::Instance().messageBoxWindow->Show("You do not have the permission.");
			break;
		case GameProtocol::GUILD_INVALID_RANK:
			Game::Instance().messageBoxWindow->Show("Incorrect Rank.\nPlease try again.");
			break;
		case GameProtocol::GUILD_ABSENT_GUILD_NAME:
			Game::Instance().messageBoxWindow->Show("This Guild does not exist.");
			break;
		case GameProtocol::GUILD_ABSENT_JOIN:
			Game::Instance().messageBoxWindow->Show("The user is not a Guild member.");
			break;
		case GameProtocol::GUILD_NEED_EMPTY_RANK:
			Game::Instance().messageBoxWindow->Show("You cannot move a member to a position already occupied by somebody else.\nPlease move to other position.");
			break;
		case GameProtocol::GUILD_MEMBER_NOT_ONLINE:
			Game::Instance().messageBoxWindow->Show("This guild member is not logged-in.");
			break;
		default:
			Game::Instance().messageBoxWindow->Show("You can't currently move a Guild member. Try again later.");
			break;
		}
	}
}

bool GuildWindow::HandleDragMouseButtonUp(const EventArgs& e)
{
	const MouseEventArgs mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton && !window_->isHit(mouseEventArgs.position))
	{
		if (mouseEventArgs.window->getUserString("type") == "rankedMember")
		{
			fromIndex_ = mouseEventArgs.window->getID();

			if (rank_ == GUILD_RANK_LEADER)
			{
				if (fromIndex_ == 0)
				{
					HandleDisbandGuild();
				}
				else
				{
					HandleRemoveMember();
				}
			}
			else
			{
				if (fromIndex_ == myIndex_)
				{
					quitting_ = true;
					HandleRemoveMember();
				}
			}
		}
		else if (mouseEventArgs.window->getUserString("type") == "member")
		{
			fromIndex_ = mouseEventArgs.window->getID() + RANKED_MEMBER_SLOT_COUNT;

			if (rank_ == GUILD_RANK_LEADER || rank_ == GUILD_RANK_FIRST)
			{
				HandleRemoveMember();
			}
			else
			{
				quitting_ = true;
				HandleRemoveMember();
			}
		}
	}
	return true;
}

void GuildWindow::HandleRemoveMember()
{
	if (quitting_)
	{
		Game::Instance().messageBoxWindow->Show("Do you really want to leave the Guild?",
			MessageBoxMode::OKCancel, Event::Subscriber(&GuildWindow::HandleRemoveMemberRankMessageBoxResult, this));
	}
	else
	{
		String memberName;
		if (IsRankedMemberSlot(*selectedMemberSlot_))
		{
			memberName = rankedMemberNameTexts_[selectedMemberSlot_->getID()]->getText();
		}
		else
		{
			memberName = memberNameTexts_[selectedMemberSlot_->getID()]->getText();
		}
		Game::Instance().messageBoxWindow->Show("Do you really want to expel " + memberName + "?",
			MessageBoxMode::OKCancel, Event::Subscriber(&GuildWindow::HandleRemoveMemberRankMessageBoxResult, this));
	}
}

bool GuildWindow::HandleRemoveMemberRankMessageBoxResult(const EventArgs& e)
{
	if (Game::Instance().messageBoxWindow->GetResult() != MessageBoxResult::OK)
	{
		return true;
	}

	processing_ = true;
	processFromIndex_ = fromIndex_;

	std::string memberName;
	if (fromIndex_ < RANKED_MEMBER_SLOT_COUNT)
	{
		memberName = rankedMemberNameTexts_[fromIndex_]->getText().c_str();
	}
	else
	{
		memberName = memberNameTexts_[fromIndex_-RANKED_MEMBER_SLOT_COUNT]->getText().c_str();
	}

	GamePacket packet(GameProtocol::PKT_GUILD_REMOVE_USER);
	packet << memberName;
	Game::Instance().gameSocket->Send(packet);
	return true;
}

void GuildWindow::HandleMemberRemoved(GamePacket& packet)
{
	processing_ = false;

	selectedMemberSlot_->removeChildWindow(memberSlotSelection_);
	selectedMemberSlot_->removeChildWindow(rankedMemberSlotSelection_);

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		if (quitting_)
		{
			window_->hide();
			return;
		}

		String memberName;
		if (processFromIndex_ < RANKED_MEMBER_SLOT_COUNT)
		{
			rankedMemberNameTexts_[processFromIndex_]->setText("");
		}
		else
		{
			memberName = memberNameTexts_[processFromIndex_-RANKED_MEMBER_SLOT_COUNT]->getText();
			memberNames_.erase(std::remove(memberNames_.begin(), memberNames_.end(), memberName), memberNames_.end());
		}

		Update();
	}
	else
	{
		sf::Uint8 errorCode;
		packet >> errorCode;

		switch (errorCode)
		{
		case GameProtocol::GUILD_NOT_GUILD_USER:
			Game::Instance().messageBoxWindow->Show("You are not one of this Guild's members.");
			break;
		case GameProtocol::GUILD_SMALL_RANK:
			Game::Instance().messageBoxWindow->Show("You do not have the permission.");
			break;
		case GameProtocol::GUILD_ABSENT_GUILD_NAME:
			Game::Instance().messageBoxWindow->Show("This Guild does not exist.");
			break;
		case GameProtocol::GUILD_ABSENT_JOIN:
			Game::Instance().messageBoxWindow->Show("The user is not a Guild member.");
			break;
		default:
			Game::Instance().messageBoxWindow->Show("You cannot currently dismiss a member from the guild. Try again later.");
			break;
		}
	}
}

void GuildWindow::HandleDisbandGuild()
{
	Game::Instance().messageBoxWindow->Show("Do you really want to disband the Guild?",
		MessageBoxMode::OKCancel, Event::Subscriber(&GuildWindow::HandleDisbandGuildMessageBoxResult, this));
}

bool GuildWindow::HandleDisbandGuildMessageBoxResult(const EventArgs& e)
{
	if (Game::Instance().messageBoxWindow->GetResult() != MessageBoxResult::OK)
	{
		return true;
	}

	processing_ = true;

	GamePacket packet(GameProtocol::PKT_GUILD_DISBAND);
	Game::Instance().gameSocket->Send(packet);
	return true;
}

void GuildWindow::HandleDisbanded(GamePacket& packet)
{
	processing_ = false;

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		window_->hide();

		Game::Instance().messageBoxWindow->Show("The Guild has been disbanded.");
	}
	else
	{
		sf::Uint8 errorCode;
		packet >> errorCode;

		switch (errorCode)
		{
		case GameProtocol::GUILD_NOT_GUILD_USER:
			Game::Instance().messageBoxWindow->Show("You are not one of this Guild's members.");
			break;
		case GameProtocol::GUILD_SMALL_RANK:
			Game::Instance().messageBoxWindow->Show("You do not have the permission.");
			break;
		case GameProtocol::GUILD_ABSENT_GUILD_NAME:
			Game::Instance().messageBoxWindow->Show("This Guild does not exist.");
			break;
		default:
			Game::Instance().messageBoxWindow->Show("You cannot currently disband the guild. Try again later.");
			break;
		}
	}
}

bool GuildWindow::HandleDismissButtonClicked(const EventArgs& e)
{
	if (!selectedMemberSlot_ || processing_)
	{
		return true;
	}

	std::string memberName;
	if (IsRankedMemberSlot(*selectedMemberSlot_))
	{
		fromIndex_ = selectedMemberSlot_->getID();
		memberName = rankedMemberNameTexts_[fromIndex_]->getText().c_str();
	}
	else if (IsMemberSlot(*selectedMemberSlot_))
	{
		fromIndex_ = selectedMemberSlot_->getID() + RANKED_MEMBER_SLOT_COUNT;
		memberName = memberNameTexts_[fromIndex_-RANKED_MEMBER_SLOT_COUNT]->getText().c_str();
	}

	if (fromIndex_ == myIndex_)
	{
		quitting_ = true;
	}
	HandleRemoveMember();
	return true;
}

bool GuildWindow::HandleGuildNameButtonClicked(const EventArgs& e)
{
	if (!processing_ && rank_ == GUILD_RANK_LEADER)
	{
		processing_ = true;
		GamePacket packet(GameProtocol::PKT_GUILD_EDIT_INFO_REQ);
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

void GuildWindow::HandleGuildEditInfoRequest(GamePacket& packet)
{
	processing_ = false;

	if (!window_->isVisible())
	{
		return;
	}

	std::string description;
	packet >> description;

	Game::Instance().guildInfoWindow->Show(guildNameButton_->getText(), description, true,
		Event::Subscriber(&GuildWindow::HandleGuildInfoResult, this));
}

void GuildWindow::HandleGuildEditInfoResult(GamePacket& packet)
{
	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		Game::Instance().messageBoxWindow->Show("Guild description has been edited.");
	}
	else
	{
		Game::Instance().messageBoxWindow->Show("Failed to edit Guild description.");
	}
}

bool GuildWindow::HandleGuildInfoResult(const EventArgs& e)
{
	if (Game::Instance().guildInfoWindow->GetResult() == GuildInfoResult::OK)
	{
		GamePacket packet(GameProtocol::PKT_GUILD_EDIT_INFO);
		packet << Game::Instance().guildInfoWindow->GetText().c_str();
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

bool GuildWindow::HandleGuildEmblemButtonClicked(const EventArgs& e)
{
	if (!processing_ && rank_ == GUILD_RANK_LEADER)
	{
		processing_ = true;
		GamePacket packet(GameProtocol::PKT_GUILD_SYMBOL_CHANGE_REQ);
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

bool GuildWindow::HandleGuildEmblemChangeResult(const EventArgs& e)
{
	if (guildEmblemChangeWindow_->GetResult() == GuildEmblemChangeResult::OK)
	{
		AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
		if (attributeComponent == nullptr)
		{
			return true;
		}

		GuildEmblem::BMP16_HEADER bmp16Header = {0};
		bmp16Header.id[0] = 'B';
		bmp16Header.id[1] = 'M';
		bmp16Header.id[2] = 'P';
		bmp16Header.id[3] = 17;
		bmp16Header.width = 16;
		bmp16Header.height = 14;

		std::string buffer;
		buffer.append(reinterpret_cast<char*>(&bmp16Header), sizeof(GuildEmblem::BMP16_HEADER));

		Imageset* imageset = &ImagesetManager::getSingleton().get("guildemblem");
		const size_t bufferSize = GuildEmblem::GUILD_EMBLEM_32BMP_DATA_SIZE; // data in 0.7.x is always RGBA 32 bit
		char* emblemBuffer = new char[bufferSize];
		imageset->getTexture()->saveToMemory(emblemBuffer);

		// Convert pixel data from 32bpp to 16bpp 565.
		// Also set the bright pink colour where there is alpha.
		char* sourceData = emblemBuffer;
		for (int i = 0; i < GuildEmblem::GUILD_EMBLEM_32BMP_DATA_SIZE; i+=4)
		{
			unsigned char r = sourceData[i];
			unsigned char g = sourceData[i+1];
			unsigned char b = sourceData[i+2];
			unsigned char a = sourceData[i+3];
			if (a != 255)
			{
				r = 255; g = 0; b = 255;
			}

			unsigned short pixel16 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
			buffer.append(reinterpret_cast<char*>(&pixel16), sizeof(pixel16));
		}

		unsigned int guildPictureId = attributeComponent->GetGuildPictureId();
		buffer.append(reinterpret_cast<char*>(&guildPictureId), sizeof(guildPictureId));

		GamePacket packet(GameProtocol::PKT_GUILD_SYMBOL_CHANGE);
		packet << boost::str(boost::format("g%1$04i001.obm") % attributeComponent->GetGuildId());
		packet << static_cast<unsigned short>(buffer.size());
		packet.Append(buffer.c_str(), buffer.size());
		Game::Instance().gameSocket->Send(packet);
	}
	return true;
}

void GuildWindow::HandleGuildEmblemChangeRequest(GamePacket& packet)
{
	processing_ = false;

	if (!window_->isVisible())
	{
		return;
	}

	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		sf::Uint32 cost;
		packet >> cost;

		if (!guildEmblemChangeWindow_->Show(cost, Event::Subscriber(&GuildWindow::HandleGuildEmblemChangeResult, this)))
		{
			Game::Instance().messageBoxWindow->Show("The picture file is not valid.");
		}
	}
	else
	{
		Game::Instance().messageBoxWindow->Show("You do not have the permission.");
	}
}

void GuildWindow::HandleGuildEmblemChangeResult(GamePacket& packet)
{
	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		sf::Uint32 guildMoney;
		packet >> guildMoney;
		guildMoney_ = guildMoney;
		guildMoneyText_->setText(PropertyHelper::uintToString(guildMoney_));
	}
	else
	{
		Game::Instance().messageBoxWindow->Show("Failed to change Guild Emblem.");
	}
}

} // namespace GUI