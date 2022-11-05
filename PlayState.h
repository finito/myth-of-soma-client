#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include "Artemis/ComponentMapper.h"

#include "Framework/GameEngine.h"

#include <boost/scoped_ptr.hpp>

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Clock.hpp>

#include "Components/AttributeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/ItemAttributeComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/DeadComponent.h"
#include "Components/ExpiresComponent.h"
#include "Components/MovementComponent.h"
#include "Components/MagicComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ChatComponent.h"

namespace artemis
{
	class World;
}

class RenderSystem;
class CameraSystem;
class MouseCursorSystem;
class TileMapRenderSystem;
class LightRenderSystem;
class AnimationSystem;
class FocusSystem;
class MovementSystem;
class ChatSystem;
class CharacterStateSystem;
class BloodSystem;
class DeathSystem;
class ExpirationSystem;
class ArrowSystem;
class MagicSystem;
class EffectSystem;
class FlyingEffectSystem;

namespace GUI
{
	class HUD;
	class HUDDead;
	class ItemTooltip;
	class EventMessageWindow;
	class EventSelectionWindow;
	class ShopWindow;
	class PosShopWindow;
	class RepairWindow;
	class MessageBoxWindow;
	class StashWindow;
	class GuildStashWindow;
	class InputBoxWindow;
	class GuildMenu;
	class GuildApplyWindow;
	class GuildWindow;
	class GuildJoinWindow;
	class InventoryWindow;
	class StatusWindow;
	class MagicAbilityWindow;
	class MagicAuraWindow;
	class MagicBlackWindow;
	class MagicBlueWindow;
	class MagicWhiteWindow;
	class MapWindow;
	class GuildInfoWindow;
	class ItemTransformWindow;
	class QuestWindow;
	class ExchangeWindow;
	class ExchangeTraderWindow;
	class GuildEmblem;
	class PrivateMessage;
	class MakeItemWindow;
	class AbilityWindow;
	class ChatWindow;
	class ChatHistoryWindow;
	class ChatInput;
}

namespace Network
{
	class GamePacket;
}

class Zone;
class PathFind;

class PlayState : public Framework::IGameState
{
public:
	PlayState(Framework::GameEngine& engine, const std::string& characterName);
	~PlayState();

protected:
	virtual void DoUpdate(float deltaTime);
	virtual void DoRender();

private:
	void HandleNetwork();
	void HandleLogin(Network::GamePacket& packetRecv);
	void SendLogin();
	void HandleEntityModify(Network::GamePacket& packetRecv);
	void SendGameStart();
	void SendLogout(bool restart);
	void HandleMove(Network::GamePacket& packetRecv, sf::Uint8 packetId);
	void SendCharacterRequest(unsigned int serverId);
	void HandleBattleMode(Network::GamePacket& packetRecv);
	void AddChat(const std::string& chatMessage, unsigned char chatType,
		CEGUI::colour chatColor = CEGUI::colour(0, 0, 0));
	void HandleChat(Network::GamePacket& packetRecv);
	bool HandleBattleModeChange(const CEGUI::EventArgs& e);
	void HandleCharacterData(Network::GamePacket& packetRecv);
	void HandleHPMPRecovery(Network::GamePacket& packetRecv);
	void HandleChangeDirection(Network::GamePacket& packetRecv);
	void SendChangeDirection(sf::Uint16 direction);
	void HandleInventoryAll(Network::GamePacket& packetRecv);
	void HandleItemUse(Network::GamePacket& packetRecv);
	void HandleChangeBeltIndex(Network::GamePacket& packetRecv);
	void HandleChangeItemInfo(Network::GamePacket& packetRecv);
	void HandleItemZoneInfo(Network::GamePacket& packetRecv);
	void HandleItemThrow(Network::GamePacket& packetRecv);
	void HandleItemPickup(Network::GamePacket& packetRecv);
	void HandleMoneyChange(Network::GamePacket& packetRecv);
	void HandleWeightChange(Network::GamePacket& packetRecv);
	void HandleMagicAll(Network::GamePacket& packetRecv);
	void HandleAddItem(Network::GamePacket& packetRecv);
	void HandleRemoveItem(Network::GamePacket& packetRecv);
	void HandleEventMessage(Network::GamePacket& packetRecv);
	void HandleEventSelection(Network::GamePacket& packetRecv);
	void HandleEventMessageBoxOK(Network::GamePacket& packetRecv);
	void HandleEquipmentItemInfo(Network::GamePacket& packetRecv);
	void HandleAttack(Network::GamePacket& packetRecv);
	void HandleDead(Network::GamePacket& packetRecv);
	void HandleZoneChange(Network::GamePacket& packetRecv);
	void HandleTownPortalReq(Network::GamePacket& packetRecv);
	void HandleTownPortalEnd(Network::GamePacket& packetRecv);
	void HandleMagicReady(Network::GamePacket& packetRecv);
	void HandleMagicArrow(Network::GamePacket& packetRecv);
	void HandlePartyInvite(Network::GamePacket& packetRecv);
	void HandlePartyInviteResult(Network::GamePacket& packetRecv);
	void HandleSetGray(Network::GamePacket& packetRecv);
	void HandleEffect(Network::GamePacket& packetRecv);
	void HandleRestartGame(Network::GamePacket& packetRecv);

	void HandleEvents();
	void HandleKeyPressed(const sf::Event& e);
	void HandleLeftMouseButtonPressed(const sf::Event& e);
	void HandleLeftMouseButtonPressing(const sf::Event& e);
	void HandleLeftMouseButtonReleased(const sf::Event& e);
	void HandleRightMouseButtonPressed(const sf::Event& e);
	void HandleRightMouseButtonPressing(const sf::Event& e);
	void HandleRightMouseButtonReleased(const sf::Event& e);

	void RotateMyCharacter();
	void MoveMyCharacter();

	bool HandleCharacterClicked(bool mousePressing);
	bool AttackCharacter();
	bool TalkToCharacter(int serverId, bool mousePressing);

	bool HandlePartyInviteWaitingMessageBoxResult(const CEGUI::EventArgs& e);
	bool HandlePartyInviteRequestMessageBoxResult(const CEGUI::EventArgs& e);

	void SendPartyInvite(bool accepted);
	void SendPartyDeny();

private:
	Framework::GameEngine& engine_;
	std::string characterName_;

	float deltaTimeRemainder_;

	boost::scoped_ptr<Zone> zone_;
	boost::scoped_ptr<PathFind> pathFind_;

	sf::RenderTexture diffuseTexture_;
	sf::RenderTexture lightTexture_;
	sf::Shader lightShader_;
	sf::Sprite diffuseSprite_;

	int myServerId_;
	std::vector<unsigned int> characterRequests_;
	bool attackFlag_;
	sf::Clock attackTimer_;
	sf::Vector2i eventClickPosition_;
	int characterClickedServerId_;

	sf::Shader grayscaleShader_;
	float grayscaleAlpha_;
	sf::Clock grayscaleFadeTimer_;

	boost::scoped_ptr<GUI::HUD>                  hud_;
	boost::scoped_ptr<GUI::HUDDead>              hudDead_;
	boost::scoped_ptr<GUI::ItemTooltip>          itemTooltip_;
	boost::scoped_ptr<GUI::EventMessageWindow>   eventMessageWindow_;
	boost::scoped_ptr<GUI::EventSelectionWindow> eventSelectionWindow_;
	boost::scoped_ptr<GUI::ShopWindow>           shopWindow_;
	boost::scoped_ptr<GUI::PosShopWindow>        posShopWindow_;
	boost::scoped_ptr<GUI::RepairWindow>         repairWindow_;
	boost::scoped_ptr<GUI::MessageBoxWindow>     messageBoxWindow_;
	boost::scoped_ptr<GUI::StashWindow>          stashWindow_;
	boost::scoped_ptr<GUI::GuildStashWindow>     guildStashWindow_;
	boost::scoped_ptr<GUI::InputBoxWindow>       inputBoxWindow_;
	boost::scoped_ptr<GUI::GuildMenu>            guildMenu_;
	boost::scoped_ptr<GUI::GuildApplyWindow>     guildApplyWindow_;
	boost::scoped_ptr<GUI::GuildWindow>          guildWindow_;
	boost::scoped_ptr<GUI::GuildJoinWindow>      guildJoinWindow_;
	boost::scoped_ptr<GUI::InventoryWindow>      inventoryWindow_;
	boost::scoped_ptr<GUI::StatusWindow>		 statusWindow_;
	boost::scoped_ptr<GUI::MagicAbilityWindow>	 magicAbilityWindow_;
	boost::scoped_ptr<GUI::MagicAuraWindow>		 magicAuraWindow_;
	boost::scoped_ptr<GUI::MagicBlackWindow>	 magicBlackWindow_;
	boost::scoped_ptr<GUI::MagicBlueWindow>		 magicBlueWindow_;
	boost::scoped_ptr<GUI::MagicWhiteWindow>     magicWhiteWindow_;
	boost::scoped_ptr<GUI::MapWindow>			 mapWindow_;
	boost::scoped_ptr<GUI::ItemTransformWindow>	 itemTransformWindow_;
	boost::scoped_ptr<GUI::GuildInfoWindow>		 guildInfoWindow_;
	boost::scoped_ptr<GUI::QuestWindow>		     questWindow_;
	boost::scoped_ptr<GUI::ExchangeWindow>		 exchangeWindow_;
	boost::scoped_ptr<GUI::ExchangeTraderWindow> exchangeTraderWindow_;
	boost::scoped_ptr<GUI::GuildEmblem>          guildEmblem_;
	boost::scoped_ptr<GUI::PrivateMessage>       privateMessage_;
	boost::scoped_ptr<GUI::MakeItemWindow>       makeItemWindow_;
	boost::scoped_ptr<GUI::AbilityWindow>        abilityWindow_;
	boost::scoped_ptr<GUI::ChatHistoryWindow>    chatHistoryWindow_;
	boost::scoped_ptr<GUI::ChatWindow>           chatWindow_;
	boost::scoped_ptr<GUI::ChatInput>            chatInput_;

	boost::scoped_ptr<artemis::World> world_;
	RenderSystem* renderSystem_;
	CameraSystem* cameraSystem_;
	MouseCursorSystem* mouseCursorSystem_;
	TileMapRenderSystem* tileMapRenderSystem_;
	LightRenderSystem* lightRenderSystem_;
	AnimationSystem* animationSystem_;
	FocusSystem* focusSystem_;
	MovementSystem* movementSystem_;
	ChatSystem* chatSystem_;
	CharacterStateSystem* characterStateSystem_;
	BloodSystem* bloodSystem_;
	DeathSystem* deathSystem_;
	ExpirationSystem* expirationSystem_;
	ArrowSystem* arrowSystem_;
	MagicSystem* magicSystem_;
	EffectSystem* effectSystem_;
	FlyingEffectSystem* flyingEffectSystem_;

	artemis::ComponentMapper<AttributeComponent> attributeMapper;
	artemis::ComponentMapper<ItemAttributeComponent> itemAttributeMapper;
	artemis::ComponentMapper<MovementComponent> movementMapper;
	artemis::ComponentMapper<MagicComponent> magicMapper;
	artemis::ComponentMapper<PositionComponent> positionMapper;
	artemis::ComponentMapper<EquipmentComponent> equipmentMapper;
	artemis::ComponentMapper<ChatComponent> chatMapper;
	artemis::ComponentMapper<InventoryComponent> inventoryMapper;

	bool renderToggle[5];
};

#endif // PLAY_STATE_H