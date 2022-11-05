
#include "GUI/QuestWindow.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <iostream>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;
using namespace Network;

QuestWindow::QuestWindow() :
progressCanScroll_(false),
noProgressCanScroll_(false)
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("questwindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&QuestWindow::HandleShown, this));
		window_->subscribeEvent(Window::EventHidden, Event::Subscriber(&QuestWindow::HandleHidden, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&QuestWindow::HandleScrollProgress, this));
		window_->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&QuestWindow::HandleScrollNoProgress, this));

		longDescription_ = window_->getChild("Quest/LongDescription");
		longDescription_->setText("");

		for (int i = 0; i < SLOT_COUNT; ++i)
		{
			progressSlots_[i] = window_->getChild("Quest/Progress" + PropertyHelper::intToString(i+1));
			progressSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&QuestWindow::HandleSlotMouseClick, this));
			progressSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&QuestWindow::HandleMouseEnterSlotArea, this));
			progressSlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&QuestWindow::HandleScrollProgress, this));

			noProgressSlots_[i] = window_->getChild("Quest/NoProgress" + PropertyHelper::intToString(i+1));
			noProgressSlots_[i]->subscribeEvent(Window::EventMouseClick,
				Event::Subscriber(&QuestWindow::HandleSlotMouseClick, this));
			noProgressSlots_[i]->subscribeEvent(Window::EventMouseEntersArea,
				Event::Subscriber(&QuestWindow::HandleMouseEnterSlotArea, this));
			noProgressSlots_[i]->subscribeEvent(Window::EventMouseWheel,
				Event::Subscriber(&QuestWindow::HandleScrollNoProgress, this));
		}

		progressScrollbar_ = static_cast<Scrollbar*>(window_->getChild("Quest/ProgressScrollbar"));
		progressScrollbar_->setStepSize(1.0f);
		progressScrollbar_->setPageSize(static_cast<float>(SLOT_COUNT));
		progressScrollbar_->setOverlapSize(0);
		progressScrollbar_->setDocumentSize(0);
		progressScrollbar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&QuestWindow::HandleScrollPositionChanged, this));
		
		noProgressScrollbar_ = static_cast<Scrollbar*>(window_->getChild("Quest/NoProgressScrollbar"));
		noProgressScrollbar_->setStepSize(1.0f);
		noProgressScrollbar_->setPageSize(static_cast<float>(SLOT_COUNT));
		noProgressScrollbar_->setOverlapSize(0);
		noProgressScrollbar_->setDocumentSize(0);
		noProgressScrollbar_->subscribeEvent(Scrollbar::EventScrollPositionChanged,
			Event::Subscriber(&QuestWindow::HandleScrollPositionChanged, this));

		closeButton_ = static_cast<PushButton*>(window_->getChild("Quest/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&QuestWindow::HandleClose, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "QuestWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

QuestWindow::~QuestWindow()
{
	window_->destroy();
}

void QuestWindow::Update()
{
	assert(Game::Instance().myEntity);
	float scrollPosition = progressScrollbar_->getScrollPosition();
	float stepSize	     = progressScrollbar_->getStepSize();
	float documentSize   = stepSize * std::ceilf(progressQuests_.size() / stepSize);

	progressScrollbar_->setDocumentSize(documentSize);
	progressScrollbar_->setScrollPosition(std::min(scrollPosition, documentSize));
	progressScrollbar_->setVisible((documentSize > SLOT_COUNT));

	unsigned int startPosition = static_cast<unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < progressQuests_.size())
		{
			progressSlots_[i]->setUserData(&progressQuests_[index]);
			progressSlots_[i]->setText(boost::str(boost::format("[colour=\'FF009600\']%1%\n[colour=\'FFFFFFFF\']%2%") %
				progressQuests_[index].title % progressQuests_[index].description));
		}
		else
		{
			progressSlots_[i]->setUserData(nullptr);
			progressSlots_[i]->setText("");
		}
	}

	scrollPosition = noProgressScrollbar_->getScrollPosition();
	stepSize	   = noProgressScrollbar_->getStepSize();
	documentSize   = stepSize * std::ceilf(noProgressQuests_.size() / stepSize);

	noProgressScrollbar_->setDocumentSize(documentSize);
	noProgressScrollbar_->setScrollPosition(std::min(scrollPosition, documentSize));
	noProgressScrollbar_->setVisible((documentSize > SLOT_COUNT));

	startPosition = static_cast<unsigned int>(stepSize * std::floorf(scrollPosition / stepSize));

	for (int i = 0; i < SLOT_COUNT; ++i)
	{
		const unsigned int index = startPosition + i;
		if (index < noProgressQuests_.size())
		{
			noProgressSlots_[i]->setUserData(&noProgressQuests_[index]);
			if (noProgressQuests_[index].isHigherLevel)
			{
				noProgressSlots_[i]->setText(boost::str(boost::format("[colour=\'FFD00000\']%1%\n[colour=\'FFFFFFFF\']%2%") %
					noProgressQuests_[index].title % noProgressQuests_[index].description));
			}
			else
			{
				noProgressSlots_[i]->setText(boost::str(boost::format("[colour=\'FFFFFF00\']%1%\n[colour=\'FFFFFFFF\']%2%") %
					noProgressQuests_[index].title % noProgressQuests_[index].description));
			}
		}
		else
		{
			noProgressSlots_[i]->setUserData(nullptr);
			noProgressSlots_[i]->setText("");
		}
	}
}

bool QuestWindow::HandleShown(const EventArgs& e)
{
	SendQuestOverviewRequest();
	Update();
	window_->setModalState(true);
	return true;
}

bool QuestWindow::HandleHidden(const EventArgs& e)
{
	window_->setModalState(false);
	return true;
}

bool QuestWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool QuestWindow::HandleScrollPositionChanged(const EventArgs& e)
{
	Update();
	return true;
}

void QuestWindow::SendQuestOverviewRequest()
{
	GamePacket packet(GameProtocol::PKT_QUEST_VIEW);
	packet << OVERVIEW;
	Game::Instance().gameSocket->Send(packet);
}

void QuestWindow::SendQuestLongDescriptionRequest(const unsigned short id)
{
	GamePacket packet(GameProtocol::PKT_QUEST_VIEW);
	packet << LONG_DESCRIPTION << id;
	Game::Instance().gameSocket->Send(packet);
}

void QuestWindow::HandleQuestData(GamePacket& packet)
{
	sf::Uint8 type;
	packet >> type;

	if (type == OVERVIEW)
	{
		progressQuests_.clear();
		noProgressQuests_.clear();

		HandleQuestOverviews(packet);
	}
	else if (type == LONG_DESCRIPTION)
	{
		HandleQuestLongDescription(packet);
	}
}

void QuestWindow::HandleQuestOverviews(GamePacket& packet)
{
	// First reads quests that are below or equal to player level then reads quests that
	// are above player level.
	//
	// Above player level quests are only sent by server if there are no quests below or
	// equal to player level that have no progress.
	for (int i = 0; i < 2; ++i)
	{
		sf::Uint16 count;
		packet >> count;

		for (int j = 0; j < count; ++j)
		{
			sf::Uint16 id;
			packet >> id;

			bool inProgress = false;
			if (i == 0)
			{
				packet >> inProgress;
			}

			const bool isHigherLevel = i == 1;

			std::string title;
			packet >> title;

			sf::Uint16 descriptionLength;
			packet >> descriptionLength;
			std::string description;
			packet.RetrieveString(description, descriptionLength);

			AddQuest(id, inProgress, isHigherLevel, title, description);
		}
	}

	progressScrollbar_->setScrollPosition(0);
	noProgressScrollbar_->setScrollPosition(0);
	Update();
}

void QuestWindow::AddQuest(const unsigned short id, const bool inProgress, const bool isHigherLevel,
	const std::string& title, const std::string& description)
{
	Quest quest;
	quest.id = id;
	quest.title = title;
	quest.description = description;
	quest.isHigherLevel = isHigherLevel;
	if (inProgress)
	{
		progressQuests_.push_back(quest);
	}
	else
	{
		noProgressQuests_.push_back(quest);
	}
}

void QuestWindow::HandleQuestLongDescription(GamePacket& packet)
{
	sf::Uint8 result;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		sf::Uint16 id;
		packet >> id;

		sf::Uint16 length;
		packet >> length;
		std::string longDescription;
		packet.RetrieveString(longDescription, length);
		boost::replace_all(longDescription, "\\\\", "\r\n\r\n");

		longDescription_->setText(longDescription);
	}
	else
	{
		longDescription_->setText("");
	}
	static_cast<Scrollbar*>(longDescription_->getChild(longDescription_->getName() + "__auto_vscrollbar__"))->setScrollPosition(0);
}

bool QuestWindow::HandleSlotMouseClick(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	if (mouseEventArgs.button == LeftButton && mouseEventArgs.window->getUserData())
	{
		const Quest& quest = *static_cast<const Quest*>(mouseEventArgs.window->getUserData());
		SendQuestLongDescriptionRequest(quest.id);
	}
	return true;
}

bool QuestWindow::HandleScrollProgress(const EventArgs& e)
{
	if (!progressCanScroll_)
	{
		return true;
	}

	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	float scrollAmount = 0;
	if (mouseEventArgs.wheelChange < 0)
	{
		scrollAmount = progressScrollbar_->getStepSize();
	}
	else if (mouseEventArgs.wheelChange > 0)
	{
		scrollAmount = -progressScrollbar_->getStepSize();
	}

	// The scroll position change event will take care of the update.
	progressScrollbar_->setScrollPosition(progressScrollbar_->getScrollPosition() + scrollAmount);
	return true;
}

bool QuestWindow::HandleScrollNoProgress(const EventArgs& e)
{
	if (!noProgressCanScroll_)
	{
		return true;
	}

	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	float scrollAmount = 0;
	if (mouseEventArgs.wheelChange < 0)
	{
		scrollAmount = noProgressScrollbar_->getStepSize();
	}
	else if (mouseEventArgs.wheelChange > 0)
	{
		scrollAmount = -noProgressScrollbar_->getStepSize();
	}

	// The scroll position change event will take care of the update.
	noProgressScrollbar_->setScrollPosition(noProgressScrollbar_->getScrollPosition() + scrollAmount);
	return true;
}

bool QuestWindow::HandleMouseEnterSlotArea(const EventArgs& e)
{
	const MouseEventArgs& mouseEventArgs = static_cast<const MouseEventArgs&>(e);
	const String& name = mouseEventArgs.window->getName();
	const String::size_type found = name.find("Quest/Progress");
	if (found != String::npos)
	{
		progressCanScroll_ = true;
		noProgressCanScroll_ = false;
	}
	else
	{
		progressCanScroll_ = false;
		noProgressCanScroll_ = true;
	}
	return true;
}

} // namespace GUI