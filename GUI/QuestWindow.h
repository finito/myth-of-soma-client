#ifndef GUI_QUEST_WINDOW_H
#define GUI_QUEST_WINDOW_H

#include <CEGUI/CEGUI.h>

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class QuestWindow
{
public:
	QuestWindow();
	~QuestWindow();

	void HandleQuestData(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);
	bool HandleClose(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleSlotMouseClick(const CEGUI::EventArgs& e);
	bool HandleScrollProgress(const CEGUI::EventArgs& e);
	bool HandleScrollNoProgress(const CEGUI::EventArgs& e);
	bool HandleMouseEnterSlotArea(const CEGUI::EventArgs& e);

	void Update();

	void SendQuestOverviewRequest();
	void SendQuestLongDescriptionRequest(unsigned short id);

	void HandleQuestOverviews(Network::GamePacket& packet);
	void AddQuest(unsigned short id, bool inProgress, const bool isHigherLevel,
		const std::string& title, const std::string& description);

	void HandleQuestLongDescription(Network::GamePacket& packet);

private:
	static const int SLOT_COUNT = 3;
	static const unsigned char OVERVIEW = 1;
	static const unsigned char LONG_DESCRIPTION = 2;

	struct Quest
	{
		unsigned short id;
		bool isHigherLevel;
		std::string title;
		std::string description;
	};

	CEGUI::Window* window_;
	CEGUI::PushButton* closeButton_;
	CEGUI::Window* longDescription_;
	CEGUI::Window* progressSlots_[SLOT_COUNT];
	CEGUI::Window* noProgressSlots_[SLOT_COUNT];
	CEGUI::Scrollbar* progressScrollbar_;
	CEGUI::Scrollbar* noProgressScrollbar_;
	bool progressCanScroll_;
	bool noProgressCanScroll_;
	std::vector<Quest> progressQuests_;
	std::vector<Quest> noProgressQuests_;

};

} // namespace GUI

#endif // GUI_QUEST_WINDOW_H