#ifndef GUI_GUILD_WINDOW_H
#define GUI_GUILD_WINDOW_H

#include <CEGUI/CEGUI.h>
#include <boost/scoped_ptr.hpp>

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class GuildEmblemChangeWindow;

class GuildWindow
{
public:
	static const int RANKED_MEMBER_SLOT_COUNT = 9;
	static const int MEMBER_SLOT_COUNT = 15;

	static const unsigned char GUILD_RANK_LEADER = 1;
	static const unsigned char GUILD_RANK_FIRST  = 2;
	static const unsigned char GUILD_RANK_SECOND = 3;
	static const unsigned char GUILD_RANK_MEMBER = 10;

	GuildWindow();
	~GuildWindow();

	void HandleMemberList(Network::GamePacket& packet);

	void HandlePositionNameChanged(Network::GamePacket& packet);

	void HandleMoney(Network::GamePacket& packet);

	void HandleMemberRankChanged(Network::GamePacket& packet);

	void HandleMemberRemoved(Network::GamePacket& packet);

	void HandleDisbanded(Network::GamePacket& packet);

	void HandleGuildEditInfoRequest(Network::GamePacket& packet);
	void HandleGuildEditInfoResult(Network::GamePacket& packet);

	void HandleGuildEmblemChangeRequest(Network::GamePacket& packet);
	void HandleGuildEmblemChangeResult(Network::GamePacket& packet);

private:
	bool HandleShown(const CEGUI::EventArgs& e);
	bool HandleHidden(const CEGUI::EventArgs& e);

	bool HandleWindowUpdated(const CEGUI::EventArgs& e);
	bool HandleScrollPositionChanged(const CEGUI::EventArgs& e);
	bool HandleScrollMembers(const CEGUI::EventArgs& e);
	bool HandleCloseButtonClicked(const CEGUI::EventArgs& e);
	bool HandleStashButtonClicked(const CEGUI::EventArgs& e);
	bool HandlePutMoney(const CEGUI::EventArgs& e);
	bool HandleGetMoney(const CEGUI::EventArgs& e);

	bool HandleRankMemberSlotMouseClick(const CEGUI::EventArgs& e);
	bool HandleRankMemberSlotInputResult(const CEGUI::EventArgs& e);

	bool HandlePutMoneyInputResult(const CEGUI::EventArgs& e);
	bool HandleGetMoneyInputResult(const CEGUI::EventArgs& e);

	void SendPutMoney(unsigned int money);
	void SendGetMoney(unsigned int money);

	bool HandleMemberSlotMouseClick(const CEGUI::EventArgs&e);

	bool HandleRankMemberSlotDragStarted(const CEGUI::EventArgs&e);
	bool HandleRankMemberSlotDragEnded(const CEGUI::EventArgs&e);

	bool HandleMemberSlotDragStarted(const CEGUI::EventArgs&e);
	bool HandleMemberSlotDragEnded(const CEGUI::EventArgs& e);

	void Update();

	void SelectRankedMemberSlot(unsigned int index);
	void SelectMemberSlot(unsigned int index);

	bool IsRankedMemberSlotEmpty(unsigned int index) const;
	bool IsMemberSlotEmpty(unsigned int index) const;

	bool HandleRankedMemberSlotDragDropItemDropped(const CEGUI::EventArgs&e);

	bool IsRankedMemberSlot(const CEGUI::Window& window) const;
	bool IsMemberSlot(const CEGUI::Window& window) const;

	void HandleChangeMemberRank(bool leader);
	bool HandleChangeMemberRankMessageBoxResult(const CEGUI::EventArgs& e);

	bool HandleDragMouseButtonUp(const CEGUI::EventArgs& e);

	void HandleRemoveMember();
	bool HandleRemoveMemberRankMessageBoxResult(const CEGUI::EventArgs& e);

	void HandleDisbandGuild();
	bool HandleDisbandGuildMessageBoxResult(const CEGUI::EventArgs& e);

	bool HandleDismissButtonClicked(const CEGUI::EventArgs& e);

	bool HandleGuildNameButtonClicked(const CEGUI::EventArgs& e);

	bool HandleGuildInfoResult(const CEGUI::EventArgs& e);

	bool HandleGuildEmblemChangeResult(const CEGUI::EventArgs& e);
	bool HandleGuildEmblemButtonClicked(const CEGUI::EventArgs& e);

private:
	CEGUI::Window*	   window_;
	CEGUI::PushButton* closeButton_;
	CEGUI::PushButton* guildEmblemButton_;
	CEGUI::Window*     guildEmblem_;
	CEGUI::PushButton* guildNameButton_;
	CEGUI::Window*	   memberCountText_;
	CEGUI::PushButton* stashButton_;
	CEGUI::PushButton* dismissButton_;
	CEGUI::Window*	   rankedMemberSlots_[RANKED_MEMBER_SLOT_COUNT];
	CEGUI::Window*	   rankedMemberTitleTexts_[RANKED_MEMBER_SLOT_COUNT];
	CEGUI::Window*	   rankedMemberNameTexts_[RANKED_MEMBER_SLOT_COUNT];
	CEGUI::Window*	   memberSlots_[MEMBER_SLOT_COUNT];
	CEGUI::Window*	   memberNameTexts_[MEMBER_SLOT_COUNT];
	CEGUI::Scrollbar*  memberScrollBar_;
	CEGUI::Window*	   moneyButton_;
	CEGUI::Window*	   guildMoneyText_;
	CEGUI::Window*	   guildMoneyButton_;
	unsigned char	   rank_;
	unsigned int	   guildMoney_;
	std::vector<std::string> memberNames_;
	CEGUI::Window*	   selectedMemberSlot_;
	CEGUI::Window*     rankedMemberSlotSelection_;
	CEGUI::Window*     memberSlotSelection_;

	CEGUI::String processString_;
	unsigned int processFromIndex_;
	unsigned int processToIndex_;
	bool processing_;
	bool quitting_;

	unsigned int fromIndex_;
	unsigned int toIndex_;
	unsigned int myIndex_;

	boost::scoped_ptr<GuildEmblemChangeWindow> guildEmblemChangeWindow_;
};

} // namespace GUI

#endif // GUI_GUILD_WINDOW_H