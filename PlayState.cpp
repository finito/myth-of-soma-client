
#include "PlayState.h"
#include "SelectCharacterState.h"

#include "Artemis/SystemManager.h"

#include <boost/format.hpp>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <iostream>

#include "LoginState.h"
#include "Game.h"
#include "Zone.h"
#include "PathFind.h"

#include "Network/GameSocket.h"
#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"
#include "Network/SessionSocket.h"

#include "MagicData.h"
#include "SpecialAttackData.h"
#include "AbilityData.h"

#include "GUI/HUD.h"
#include "GUI/HUDDead.h"
#include "GUI/CharacterFocus.h"
#include "GUI/ItemFocus.h"
#include "GUI/ItemTooltip.h"
#include "GUI/InventoryWindow.h"
#include "GUI/EventMessageWindow.h"
#include "GUI/EventSelectionWindow.h"
#include "GUI/ShopWindow.h"
#include "GUI/PosShopWindow.h"
#include "GUI/RepairWindow.h"
#include "GUI/MessageBoxWindow.h"
#include "GUI/StashWindow.h"
#include "GUI/GuildStashWindow.h"
#include "GUI/InputBoxWindow.h"
#include "GUI/GuildMenu.h"
#include "GUI/GuildApplyWindow.h"
#include "GUI/GuildWindow.h"
#include "GUI/GuildJoinWindow.h"
#include "GUI/InventoryWindow.h"
#include "GUI/StatusWindow.h"
#include "GUI/MagicAbilityWindow.h"
#include "GUI/MagicAuraWindow.h"
#include "GUI/MagicBlackWindow.h"
#include "GUI/MagicBlueWindow.h"
#include "GUI/MagicWhiteWindow.h"
#include "GUI/MapWindow.h"
#include "GUI/GuildInfoWindow.h"
#include "GUI/ItemTransformWindow.h"
#include "GUI/QuestWindow.h"
#include "GUI/ExchangeWindow.h"
#include "GUI/ExchangeTraderWindow.h"
#include "GUI/GuildEmblem.h"
#include "GUI/PrivateMessage.h"
#include "GUI/MakeItemWindow.h"
#include "GUI/AbilityWindow.h"
#include "GUI/ChatWindow.h"
#include "GUI/ChatHistoryWindow.h"
#include "GUI/ChatInput.h"

#include "Systems/CameraSystem.h"
#include "Systems/MouseCursorSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/FocusSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ChatSystem.h"
#include "Systems/CharacterStateSystem.h"
#include "Systems/BloodSystem.h"
#include "Systems/DeathSystem.h"
#include "Systems/ExpirationSystem.h"
#include "Systems/ArrowSystem.h"
#include "Systems/MagicSystem.h"
#include "Systems/EffectSystem.h"
#include "Systems/FlyingEffectSystem.h"

#include "Systems/Rendering/RenderSystem.h"
#include "Systems/Rendering/TileMapRenderSystem.h"
#include "Systems/Rendering/LightRenderSystem.h"

#include "Framework/Utils/PositionConvert.h"
#include "Framework/Assets/AsyncFileLoad.h"
#include "Framework/Assets/ResourceCache.h"
#include "Framework/Graphics/TextureRegion.h"

#include "EntityFactory.h"

#include "DataTables/ItemTable.h"
#include "DataTables/MagicTable.h"

using namespace artemis;

PlayState::PlayState(Framework::GameEngine& engine, const std::string& characterName) :
engine_(engine),
characterName_(characterName),
deltaTimeRemainder_(0),
zone_(new Zone),
pathFind_(new PathFind),
myServerId_(-1),
attackFlag_(false),
characterClickedServerId_(-1),
grayscaleAlpha_(0),
world_(new World),
itemTooltip_(new GUI::ItemTooltip),
hud_(new GUI::HUD),
hudDead_(new GUI::HUDDead),
eventMessageWindow_(new GUI::EventMessageWindow),
eventSelectionWindow_(new GUI::EventSelectionWindow),
shopWindow_(new GUI::ShopWindow),
posShopWindow_(new GUI::PosShopWindow),
repairWindow_(new GUI::RepairWindow),
messageBoxWindow_(new GUI::MessageBoxWindow),
stashWindow_(new GUI::StashWindow),
guildStashWindow_(new GUI::GuildStashWindow),
inputBoxWindow_(new GUI::InputBoxWindow),
guildMenu_(new GUI::GuildMenu),
guildApplyWindow_(new GUI::GuildApplyWindow),
guildWindow_(new GUI::GuildWindow),
guildJoinWindow_(new GUI::GuildJoinWindow),
inventoryWindow_(new GUI::InventoryWindow),
statusWindow_(new GUI::StatusWindow),
magicAbilityWindow_(new GUI::MagicAbilityWindow),
magicAuraWindow_(new GUI::MagicAuraWindow),
magicBlackWindow_(new GUI::MagicBlackWindow),
magicBlueWindow_(new GUI::MagicBlueWindow),
magicWhiteWindow_(new GUI::MagicWhiteWindow),
mapWindow_(new GUI::MapWindow),
itemTransformWindow_(new GUI::ItemTransformWindow),
guildInfoWindow_(new GUI::GuildInfoWindow),
questWindow_(new GUI::QuestWindow),
exchangeWindow_(new GUI::ExchangeWindow),
exchangeTraderWindow_(new GUI::ExchangeTraderWindow),
guildEmblem_(new GUI::GuildEmblem),
privateMessage_(new GUI::PrivateMessage),
makeItemWindow_(new GUI::MakeItemWindow),
abilityWindow_(new GUI::AbilityWindow),
chatHistoryWindow_(new GUI::ChatHistoryWindow),
chatWindow_(new GUI::ChatWindow),
chatInput_(new GUI::ChatInput)
{
	for (int i = 0; i < 5; ++i)
	{
		renderToggle[i] = true;
	}

	diffuseTexture_.create(engine_.renderWindow.getSize().x, engine_.renderWindow.getSize().y);
	diffuseSprite_.setTexture(diffuseTexture_.getTexture());

	lightTexture_.create(engine_.renderWindow.getSize().x, engine_.renderWindow.getSize().y);
	lightShader_.loadFromFile("shaders/light.frag", sf::Shader::Fragment);
	lightShader_.setParameter("diffuseTexture", sf::Shader::CurrentTexture);
	lightShader_.setParameter("lightMapTexture", lightTexture_.getTexture());

	grayscaleShader_.loadFromFile("shaders/grayscale.frag", sf::Shader::Fragment);
	grayscaleShader_.setParameter("quadTexture", sf::Shader::CurrentTexture);
	grayscaleShader_.setParameter("alpha", 0.0f);

	SystemManager* systemManager = world_->getSystemManager();
	EntityManager* entityManager = world_->getEntityManager();

	renderSystem_ = dynamic_cast<RenderSystem*>(systemManager->setSystem(new RenderSystem(*zone_, diffuseTexture_)));

	cameraSystem_ = dynamic_cast<CameraSystem*>(systemManager->setSystem(new CameraSystem(engine_.renderWindow)));

	mouseCursorSystem_ = dynamic_cast<MouseCursorSystem*>(systemManager->setSystem(new MouseCursorSystem(engine_.renderWindow)));

	tileMapRenderSystem_ = dynamic_cast<TileMapRenderSystem*>(systemManager->setSystem(new TileMapRenderSystem(diffuseTexture_, *zone_.get())));

	animationSystem_ = dynamic_cast<AnimationSystem*>(systemManager->setSystem(new AnimationSystem()));

	lightRenderSystem_ = dynamic_cast<LightRenderSystem*>(systemManager->setSystem(new LightRenderSystem(lightTexture_, *zone_->GetMapLight())));

	focusSystem_ = dynamic_cast<FocusSystem*>(systemManager->setSystem(new FocusSystem(*zone_.get(), engine_.renderWindow)));

	movementSystem_ = dynamic_cast<MovementSystem*>(systemManager->setSystem(new MovementSystem(*zone_.get())));

	chatSystem_ = dynamic_cast<ChatSystem*>(systemManager->setSystem(new ChatSystem()));

	characterStateSystem_ = dynamic_cast<CharacterStateSystem*>(systemManager->setSystem(new CharacterStateSystem()));

	bloodSystem_ = dynamic_cast<BloodSystem*>(systemManager->setSystem(new BloodSystem()));

	deathSystem_ = dynamic_cast<DeathSystem*>(systemManager->setSystem(new DeathSystem()));

	expirationSystem_ = dynamic_cast<ExpirationSystem*>(systemManager->setSystem(new ExpirationSystem(*zone_.get())));

	arrowSystem_ = dynamic_cast<ArrowSystem*>(systemManager->setSystem(new ArrowSystem()));

	magicSystem_ = dynamic_cast<MagicSystem*>(systemManager->setSystem(new MagicSystem(*zone_.get())));

	effectSystem_ = dynamic_cast<EffectSystem*>(systemManager->setSystem(new EffectSystem()));

	flyingEffectSystem_ = dynamic_cast<FlyingEffectSystem*>(systemManager->setSystem(new FlyingEffectSystem()));

	systemManager->initializeAll();

	attributeMapper.init(*world_);
	itemAttributeMapper.init(*world_);
	movementMapper.init(*world_);
	magicMapper.init(*world_);
	positionMapper.init(*world_);
	equipmentMapper.init(*world_);
	chatMapper.init(*world_);
	inventoryMapper.init(*world_);

	Game::Instance().gui = &engine_.gui;
	Game::Instance().itemTooltip = itemTooltip_.get();
	Game::Instance().messageBoxWindow = messageBoxWindow_.get();
	Game::Instance().inputBoxWindow = inputBoxWindow_.get();
	Game::Instance().guildInfoWindow = guildInfoWindow_.get();
	Game::Instance().guildEmblem = guildEmblem_.get();

	SendLogin();
}

PlayState::~PlayState()
{
	Game::Instance().Cleanup();
}

void PlayState::DoUpdate(const float deltaTime)
{
	//Framework::PollAsyncLoads();
	//Framework::textureCache.CheckLoad();
	//Framework::textureAtlasCache.CheckLoad();
	//Framework::animationCache.CheckLoad();

	engine_.gui.GetSystem().injectTimePulse(deltaTime);
	Game::Instance().sessionSocket->SendKeepAlive();
	HandleNetwork();
	HandleEvents();

	world_->loopStart();
	world_->setDelta(deltaTime);

	if (Game::Instance().myEntity != nullptr)
	{
		expirationSystem_->process();
		//inputSystem_->process();
		movementSystem_->process();
		cameraSystem_->process();
		chatSystem_->process();
		focusSystem_->process();
		arrowSystem_->process();
		magicSystem_->process();
		effectSystem_->process();
		flyingEffectSystem_->process();
		bloodSystem_->process();
		deathSystem_->process();
		characterStateSystem_->process();
		animationSystem_->process();
		mouseCursorSystem_->process();
	}
}

void PlayState::DoRender()
{
	if (Game::Instance().myEntity == nullptr)
	{
		return;
	}

	engine_.renderWindow.pushGLStates();
	diffuseTexture_.clear();
	if (renderToggle[0])
	{
		tileMapRenderSystem_->process();
	}
	if (renderToggle[1])
	{
		renderSystem_->process();
	}

	diffuseTexture_.display();

	lightTexture_.clear();
	if (renderToggle[2])
	{
		lightRenderSystem_->process();
	}
	lightTexture_.display();

	sf::RenderStates states;
	states.shader = &lightShader_;
	engine_.renderWindow.draw(diffuseSprite_, states);

	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent != nullptr && !attributeComponent->IsAlive())
	{
		if (grayscaleAlpha_ < 1.0f && grayscaleFadeTimer_.getElapsedTime().asMilliseconds() > 250)
		{
			grayscaleAlpha_ += 0.1f;
			grayscaleShader_.setParameter("alpha", grayscaleAlpha_);
			grayscaleFadeTimer_.restart();
		}
		states.shader = &grayscaleShader_;
		engine_.renderWindow.draw(diffuseSprite_, states);
	}
	engine_.renderWindow.popGLStates();

	if (renderToggle[3])
	{
		engine_.gui.GetSystem().renderGUI();
	}
}

void PlayState::HandleNetwork()
{
	// Attempt to receive a packet
	Network::GamePacket packet;
	while (Game::Instance().gameSocket->Receive(packet) == Network::GameSocket::Done)
	{
		// Do something if received a packet
		sf::Uint8 packetId;
		packet >> packetId;

		switch (packetId)
		{
		case Network::GameProtocol::PKT_LOGIN:
			HandleLogin(packet);
			break;
		case Network::GameProtocol::PKT_USERMODIFY:
			HandleEntityModify(packet);
			break;
		case Network::GameProtocol::PKT_CHARACTER_DATA:
			HandleCharacterData(packet);
			break;
		case Network::GameProtocol::PKT_ITEM_FIELD_INFO:
			HandleItemZoneInfo(packet);
			break;
		}

		// Do not allow game related packets until the character for this client has been loaded.
		if (Game::Instance().myEntity == nullptr)
		{
			return;
		}

		switch (packetId)
		{
		case Network::GameProtocol::PKT_INV_ALL:
			HandleInventoryAll(packet);
			break;
		case Network::GameProtocol::PKT_SET_TIME:
			//HandleSetTime(packet);
			break;
		case Network::GameProtocol::PKT_MAGIC_ALL:
			HandleMagicAll(packet);
			break;
		//case Network::GameProtocol::PKT_CONTRIBUTION_STATUS:
		//	break;
		case Network::GameProtocol::PKT_ALLCHAT_STATUS:
			//HandleAllChatStatus(packet);
			break;
		case Network::GameProtocol::PKT_MOVEFIRST:
		case Network::GameProtocol::PKT_MOVEMIDDLE:
		case Network::GameProtocol::PKT_MOVEEND:
		case Network::GameProtocol::PKT_RUN_MOVEFIRST:
		case Network::GameProtocol::PKT_RUN_MOVEMIDDLE:
		case Network::GameProtocol::PKT_RUN_MOVEEND:
			HandleMove(packet, packetId);
			break;
		case Network::GameProtocol::PKT_BATTLEMODE:
			HandleBattleMode(packet);
			break;
		case Network::GameProtocol::PKT_CHAT:
			HandleChat(packet);
			break;
		case Network::GameProtocol::PKT_HPMP_RECOVERY:
			HandleHPMPRecovery(packet);
			break;
		case Network::GameProtocol::PKT_CHANGEDIR:
			HandleChangeDirection(packet);
			break;
		case Network::GameProtocol::PKT_CHANGE_ITEM_INDEX:
			inventoryWindow_->HandleChangeItemIndex(packet);
			break;
		case Network::GameProtocol::PKT_ITEM_USE:
			HandleItemUse(packet);
			break;
		case Network::GameProtocol::PKT_PUTITEM_BELT:
			hud_->HandlePutItemBelt(packet);
			break;
		case Network::GameProtocol::PKT_PUTITEM_INV:
			inventoryWindow_->HandlePutItemInv(packet);
			break;
		case Network::GameProtocol::PKT_CHANGEBELTINDEX:
			HandleChangeBeltIndex(packet);
			break;
		case Network::GameProtocol::PKT_ITEM_CHANGE_INFO:
			HandleChangeItemInfo(packet);
			break;
		case Network::GameProtocol::PKT_ITEM_THROW:
			HandleItemThrow(packet);
			break;
		case Network::GameProtocol::PKT_ITEM_PICKUP:
			HandleItemPickup(packet);
			break;
		case Network::GameProtocol::PKT_MONEYCHANGE:
			HandleMoneyChange(packet);
			break;
		case Network::GameProtocol::PKT_WEIGHT:
			HandleWeightChange(packet);
			break;
		case Network::GameProtocol::PKT_EVENTSELBOX:
			HandleEventSelection(packet);
			break;
		case Network::GameProtocol::PKT_EVENTOKBOX:
			HandleEventMessageBoxOK(packet);
			break;
		case Network::GameProtocol::PKT_EVENTNORMAL:
			HandleEventMessage(packet);
			break;
		case Network::GameProtocol::PKT_SHOPOPEN:
			shopWindow_->HandleOpenShop(packet);
			break;
		case Network::GameProtocol::PKT_SELL_SHOP:
			engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventShopSell);
			break;
		case Network::GameProtocol::PKT_BUY_SHOP:
			engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventShopBuy);
			break;
		case Network::GameProtocol::PKT_GETITEM:
			HandleAddItem(packet);
			break;
		case Network::GameProtocol::PKT_OUTITEM:
			HandleRemoveItem(packet);
			break;
		case Network::GameProtocol::PKT_SHOP_ITEM_COUNT:
			shopWindow_->HandleShopItemQuantity(packet);
			break;
		case Network::GameProtocol::PKT_REPAIR_OPEN:
			CEGUI::System::getSingleton().getGUISheet()->getChild("Repair/Frame")->show();
			break;
		case Network::GameProtocol::PKT_REPAIR_REQ:
			repairWindow_->HandleRepairItem(packet);
			break;
		case Network::GameProtocol::PKT_STORAGEOPEN:
			stashWindow_->HandleOpenStorage(packet);
			break;
		case Network::GameProtocol::PKT_SAVEITEM:
			stashWindow_->HandlePutItem(packet);
			break;
		case Network::GameProtocol::PKT_TAKEBACKITEM:
			stashWindow_->HandleGetItem(packet);
			break;
		case Network::GameProtocol::PKT_SAVEMONEY: // Fall-through
		case Network::GameProtocol::PKT_TAKEBACKMONEY:
			stashWindow_->HandlePutGetMoney(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_OPEN:
			guildMenu_->HandleGuildOpen(packet, eventClickPosition_);
			break;
		case Network::GameProtocol::PKT_GUILD_REMOVE_REQ:
			guildMenu_->HandleCancelApplyGuild(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_APPLICATION_LIST:
			guildJoinWindow_->HandleApplicationList(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_MEMBER_LIST:
			guildWindow_->HandleMemberList(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_MOVE_REQ:
			guildJoinWindow_->HandleApplicationJoin(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_MOVE_REJECT:
			guildJoinWindow_->HandleApplicationReject(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_REQ_USER_INFO:
			guildJoinWindow_->HandleRequestPlayerInfo(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_LIST:
			guildApplyWindow_->HandleGuildList(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_INFO:
			guildApplyWindow_->HandleRequestGuildInfo(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_REQ:
			guildApplyWindow_->HandleApplyToGuild(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_NEW:
			guildMenu_->HandleMakeGuild(packet);
			break;
		case Network::GameProtocol::PKT_ITEM_INFO:
			HandleEquipmentItemInfo(packet);
			break;
		case Network::GameProtocol::PKT_ATTACK:
			HandleAttack(packet);
			break;
		case Network::GameProtocol::PKT_DEAD:
			HandleDead(packet);
			break;
		case Network::GameProtocol::PKT_ZONECHANGE:
			HandleZoneChange(packet);
			break;
		case Network::GameProtocol::PKT_TOWNPORTALREQ:
			HandleTownPortalReq(packet);
			break;
		case Network::GameProtocol::PKT_TOWNPORTALEND:
			HandleTownPortalEnd(packet);
			break;
		case Network::GameProtocol::PKT_MAGIC_READY:
			HandleMagicReady(packet);
			break;
		case Network::GameProtocol::PKT_ATTACK_MAGIC_ARROW:
			HandleMagicArrow(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_STORAGEOPEN:
			guildStashWindow_->HandleOpenGuildStorage(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_SAVEITEM:
			guildStashWindow_->HandlePutItem(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_TAKEBACKITEM:
			guildStashWindow_->HandleGetItem(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_STORAGE_LIMIT_CHANGE:
			guildStashWindow_->HandleLimitChange(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_CHANGE_POS:
			guildWindow_->HandlePositionNameChanged(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_MONEY:
			guildWindow_->HandleMoney(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_CHANGE_RANK:
			guildWindow_->HandleMemberRankChanged(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_REMOVE_USER:
			guildWindow_->HandleMemberRemoved(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_DISBAND:
			guildWindow_->HandleDisbanded(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_EDIT_INFO_REQ:
			guildWindow_->HandleGuildEditInfoRequest(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_EDIT_INFO:
			guildWindow_->HandleGuildEditInfoResult(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_SYMBOL_CHANGE_REQ:
			guildWindow_->HandleGuildEmblemChangeRequest(packet);
			break;
		case Network::GameProtocol::PKT_GUILD_SYMBOL_CHANGE:
			guildWindow_->HandleGuildEmblemChangeResult(packet);
			break;
		case Network::GameProtocol::PKT_CLASSPOINTBUY:
			posShopWindow_->HandlePosShop(packet);
			break;
		case Network::GameProtocol::PKT_CHANGE_OTHER_ITEM:
			{
			sf::Uint8 type; packet >> type;
			if (type == 0) itemTransformWindow_->HandleItemList(packet);
			else if (type == 1) itemTransformWindow_->HandleItemTransform(packet);
			}
			break;
		case Network::GameProtocol::PKT_PARTY_INVITE:
			HandlePartyInvite(packet);
			break;
		case Network::GameProtocol::PKT_PARTY_INVITE_RESULT:
			HandlePartyInviteResult(packet);
			break;
		case Network::GameProtocol::PKT_MESSAGE:
			{
			sf::Uint16 messageId; packet >> messageId;
			if (messageId == 24)
			{
				Network::GamePacket packetRecv(Network::GameProtocol::PKT_PARTY_INVITE_RESULT);
				packetRecv << Network::GameProtocol::INVITE_FAIL;
				HandlePartyInviteResult(packetRecv);
			}
			}
			break;
		case Network::GameProtocol::PKT_QUEST_VIEW:
			questWindow_->HandleQuestData(packet);
			break;
		case Network::GameProtocol::PKT_TRADE_REQ:
			exchangeWindow_->HandleTradeRequest(packet);
			break;
		case Network::GameProtocol::PKT_TRADE_ACK:
			exchangeWindow_->HandleTradeAccept(packet);
			break;
		case Network::GameProtocol::PKT_TRADE_OK:
			exchangeWindow_->HandleTradeOK(packet);
			break;
		case Network::GameProtocol::PKT_TRADE_RESULT:
			exchangeWindow_->HandleTradeResult(packet);
			break;
		case Network::GameProtocol::PKT_TRADE_CANCEL:
			exchangeWindow_->HandleTradeCancel(packet);
			break;
		case Network::GameProtocol::PKT_TRADE_SETMONEY:
			exchangeWindow_->HandleTradeMoney(packet);
			break;
		case Network::GameProtocol::PKT_TRADE_ADDITEM:
			exchangeWindow_->HandleTradeItem(packet);
			break;
		case Network::GameProtocol::PKT_TRADEREXCHANGE:
			{
				sf::Uint8 type; packet >> type;
				if (type == 1) exchangeTraderWindow_->HandleOpenExchangeTrader(packet);
			}
			break;
		case Network::GameProtocol::PKT_GUILD_SYMBOL_DATA:
			guildEmblem_->HandleGuildEmblem(packet);
			break;
		case Network::GameProtocol::PKT_EFFECT:
			HandleEffect(packet);
			break;
		case Network::GameProtocol::PKT_MAKEROPEN:
			makeItemWindow_->HandleMakeItemOpen(packet);
			break;
		case Network::GameProtocol::PKT_MAKEITEM:
			makeItemWindow_->HandleProduceItem(packet);
			break;
		case Network::GameProtocol::PKT_RESTARTGAME:
			HandleRestartGame(packet);
			break;
		case Network::GameProtocol::PKT_SKILL_ABILITY:
			{
				sf::Uint8 type; packet >> type;
				switch (type)
				{
				case Network::GameProtocol::ABILITY_OPEN:
					abilityWindow_->HandleAbilityOpen(packet);
					break;
				case Network::GameProtocol::ABILITY_DISASSEMBLE:
				case Network::GameProtocol::ABILITY_SYNTHESIS:
				case Network::GameProtocol::ABILITY_SMELT:
				case Network::GameProtocol::ABILITY_PREPARE:
				case Network::GameProtocol::ABILITY_UPGRADE:
					abilityWindow_->HandleProduceItem(packet);
					break;
				}
			}
			break;
		default:
			break;
		}
	}
}

void PlayState::HandleLogin(Network::GamePacket& packetRecv)
{
	sf::Uint8 result = Network::GameProtocol::FAIL;
	packetRecv >> result;

	if (result == Network::GameProtocol::SUCCESS)
	{
		packetRecv >> myServerId_;

		sf::Int16 x;
		packetRecv >> x;

		sf::Int16 y;
		packetRecv >> y;

		sf::Int8 zone;
		packetRecv >> zone;

		// TODO: Set the zone and position within it.
		zone_->Load(zone);
	}
	else
	{
		// TODO: Disconnection message before quit.
		// Possibly look at going back to login instead of completely closing.
		engine_.Quit();
	}
}

void PlayState::SendLogin()
{
	Network::GamePacket packet(Network::GameProtocol::PKT_LOGIN);
	packet << characterName_;
	Game::Instance().gameSocket->Send(packet);
}

void PlayState::HandleEntityModify(Network::GamePacket& packetRecv)
{
	sf::Uint8 type;
	packetRecv >> type;

	sf::Uint32 serverId;
	packetRecv >> serverId;

	sf::Uint16 classType;
	packetRecv >> classType;

	sf::Int16 positionX;
	packetRecv >> positionX;

	sf::Int16 positionY;
	packetRecv >> positionY;

	if (type == Network::GameProtocol::INFO_MODIFY)
	{
		bool canAttack;
		packetRecv >> canAttack;

		std::string name;
		packetRecv >> name;

		sf::Uint8 unknown1;
		packetRecv >> unknown1;

		std::string guildName;
		packetRecv >> guildName;

		sf::Uint16 guildPictureId;
		packetRecv >> guildPictureId;

		sf::Uint8 guildRank;
		packetRecv >> guildRank;

		bool inGuildWar;
		packetRecv >> inGuildWar;

		sf::Int16 guildTownWarType;
		packetRecv >> guildTownWarType;

		sf::Int16 guildId;
		packetRecv >> guildId;

		sf::Int16 guildWarGuildId;
		packetRecv >> guildWarGuildId;

		bool guildWarDied;
		packetRecv >> guildWarDied;

		std::string partyLeaderName;
		packetRecv >> partyLeaderName;

		bool worldWar;
		packetRecv >> worldWar;

		sf::Uint16 maximumHealth;
		packetRecv >> maximumHealth;

		sf::Uint16 health;
		packetRecv >> health;

		sf::Uint16 hair;
		packetRecv >> hair;

		sf::Uint16 hairMode;
		packetRecv >> hairMode;

		sf::Uint16 skin;	// Used for blood type by NPC
		packetRecv >> skin;

		sf::Uint16 gender; // Used for the pic/skin by NPC
		packetRecv >> gender;

		sf::Uint16 moral;
		packetRecv >> moral;

		bool isAlive;
		packetRecv >> isAlive;

		bool pkEnabled;
		packetRecv >> pkEnabled; // Not much point in using it.

		sf::Uint8 battleMode;
		packetRecv >> battleMode;

		sf::Uint16 weaponSpeed;
		packetRecv >> weaponSpeed;

		sf::Uint16 direction;
		packetRecv >> direction;

		sf::Uint8 grayMode;
		packetRecv >> grayMode;

		sf::Int16 equippedItemPictureId[Network::GameProtocol::EQUIP_ITEM_NUM];
		sf::Uint16 equippedItemType[Network::GameProtocol::EQUIP_ITEM_NUM];
		sf::Uint16 equippedItemArm[Network::GameProtocol::EQUIP_ITEM_NUM];
		for (int i = 0; i < Network::GameProtocol::EQUIP_ITEM_NUM; ++i)
		{
			packetRecv >> equippedItemPictureId[i];
			if (equippedItemPictureId[i] == -1) equippedItemPictureId[i] = 0;
			packetRecv >> equippedItemType[i];
			packetRecv >> equippedItemArm[i];
		}

		sf::Int16 unknown2;
		packetRecv >> unknown2;

		bool isMe = false;
		if (serverId == myServerId_)
		{
			isMe = true;
		}
		bool isModify = false;

		artemis::Entity* character = zone_->GetServerEntity(serverId);
		if (character)
		{
			isModify = true;
		}
		else
		{
			character = &EntityFactory::CreateCharacter(*world_, sf::Vector2i(positionX, positionY));

			if (serverId < 10000)
			{
				character->addComponent(new EquipmentComponent());
				character->refresh();
			}

			if (isMe)
			{
				Game::Instance().myEntity = character;
				world_->getTagManager()->subscribe("PLAYER", *character);
				SendGameStart();
			}
			zone_->AddServerEntity(serverId, *character);
			characterRequests_.erase(
				std::remove(characterRequests_.begin(), characterRequests_.end(), serverId),
				characterRequests_.end());
		}

		bool moreMagic;
		while ((packetRecv >> moreMagic))
		{
			sf::Uint16 magicId;
			packetRecv >> magicId;

			sf::Uint32 remainingTime;
			packetRecv >> remainingTime;

			const MagicTable::MagicElement* magicTable = Game::Instance().magicTable->Get(magicId);
			if (magicTable != nullptr)
			{
				if (magicTable->endPreMagic != 0)
				{
					magicSystem_->CreateEffectEntity(*magicTable, *character, magicTable->endPreMagic, EffectType::Pre, remainingTime);
				}

				if (magicTable->endPostMagic != 0)
				{
					magicSystem_->CreateEffectEntity(*magicTable, *character, magicTable->endPostMagic, EffectType::Post, remainingTime);
				}
			}
		}

		bool moreSpecialAttack;
		while ((packetRecv >> moreSpecialAttack))
		{
			sf::Uint16 specialAttackId;
			packetRecv >> specialAttackId;

			sf::Uint32 remainingTime;
			packetRecv >> remainingTime;
		}

		AttributeComponent* attributeComponent = attributeMapper.get(*character);
		PositionComponent* positionComponent = positionMapper.get(*character);
		if (attributeComponent != nullptr && positionComponent != nullptr)
		{
			attributeComponent->SetServerId(serverId);
			attributeComponent->SetName(boost::to_upper_copy(name));
			attributeComponent->SetGuildName(boost::to_upper_copy(guildName));
			attributeComponent->SetGuildId(guildId);
			attributeComponent->SetGuildPictureId(guildPictureId);
			attributeComponent->SetSkin(serverId < 10000 ? skin : gender);
			attributeComponent->SetDirection(static_cast<unsigned char>(direction));
			if (serverId < 10000)
			{
				attributeComponent->SetGender(gender == 0 ? Gender::Female : Gender::Male);
			}
			attributeComponent->SetBattleMode(battleMode);
			attributeComponent->SetState(StateType::Idle);
			attributeComponent->SetAlive(isAlive);
			attributeComponent->SetAttackable(canAttack);
			attributeComponent->SetHair(hair);
			attributeComponent->SetClassType(classType);
			attributeComponent->SetBloodType(skin);
			attributeComponent->SetHealth(health);
			attributeComponent->SetMaximumHealth(maximumHealth);
			attributeComponent->SetMoral(moral);
			attributeComponent->SetPartyLeaderName(partyLeaderName);
			attributeComponent->SetGrayMode(grayMode);

			EquipmentComponent* equipmentComponent = equipmentMapper.get(*character);
			if (equipmentComponent != nullptr)
			{
				equipmentComponent->pictureId[ArmatureType::Top] = equippedItemPictureId[ItemEquipPosition::Chest];
				equipmentComponent->pictureId[ArmatureType::HelmetOrHair] = equippedItemPictureId[ItemEquipPosition::Head];
				equipmentComponent->pictureId[ArmatureType::Boot] = equippedItemPictureId[ItemEquipPosition::Foot];
				equipmentComponent->pictureId[ArmatureType::Pad] = equippedItemPictureId[ItemEquipPosition::Leg];
				equipmentComponent->pictureId[ArmatureType::Shield] = equippedItemPictureId[ItemEquipPosition::LeftHand];
				equipmentComponent->pictureId[ArmatureType::Weapon] = equippedItemPictureId[ItemEquipPosition::RightHand];
			}

			if (isMe)
			{
				if (attributeComponent->IsAlive())
				{
					hudDead_->Hide();
					hud_->Show();
				}

				positionComponent->SetCellPosition(sf::Vector2i(positionX, positionY));
				MovementComponent* movementComponent = movementMapper.get(*character);
				if (movementComponent != nullptr)
				{
					while (!movementComponent->GetQueue().empty()) movementComponent->GetQueue().pop();
					movementComponent->SetStartCellPosition(positionComponent->GetCellPosition());
					movementComponent->SetNextCellPosition(positionComponent->GetCellPosition());
				}
			}
			else
			{
				if (isModify) zone_->SetMovable(positionComponent->GetCellPosition(), true);
				positionComponent->SetCellPosition(sf::Vector2i(positionX, positionY));
				zone_->SetMovable(positionComponent->GetCellPosition(), false);
			}
		}
	}
	else if (type == Network::GameProtocol::INFO_DELETE)
	{
		if (serverId != myServerId_)
		{
			Entity* character = zone_->GetServerEntity(serverId);
			if (character != nullptr)
			{
				PositionComponent* positionComponent = positionMapper.get(*character);
				if (positionComponent != nullptr)
				{
					zone_->SetMovable(positionComponent->GetCellPosition(), true);
				}
				world_->deleteEntity(*character);
			}
			zone_->RemoveServerEntity(serverId);
		}
	}
}

void PlayState::SendGameStart()
{
	Network::GamePacket packet(Network::GameProtocol::PKT_GAMESTART);
	Game::Instance().gameSocket->Send(packet);
}

void PlayState::SendLogout(bool restart)
{
	Network::GamePacket packet(Network::GameProtocol::PKT_LOGOUT);
	packet << static_cast<sf::Uint16>(restart);	// 0 = Full logout 1 = Logout to Character selection
	Game::Instance().gameSocket->Send(packet);
}

void PlayState::HandleMove(Network::GamePacket& packetRecv, const sf::Uint8 packetId)
{
	sf::Uint8 result;
	packetRecv >> result;

	sf::Uint32 serverId;
	packetRecv >> serverId;

	sf::Int16 worldWarAreaOwner;
	packetRecv >> worldWarAreaOwner;

	sf::Int16 destinationX;
	packetRecv >> destinationX;

	sf::Int16 destinationY;
	packetRecv >> destinationY;

	sf::Int16 firstX = -1;
	sf::Int16 firstY = -1;
	switch (packetId)
	{
	case Network::GameProtocol::PKT_MOVEFIRST:
	case Network::GameProtocol::PKT_RUN_MOVEFIRST:
		packetRecv >> firstX;
		packetRecv >> firstY;
		break;
	}

	sf::Int16 nextX = -1;
	sf::Int16 nextY = -1;
	switch (packetId)
	{
	case Network::GameProtocol::PKT_RUN_MOVEFIRST:
	case Network::GameProtocol::PKT_RUN_MOVEMIDDLE:
	case Network::GameProtocol::PKT_RUN_MOVEEND:
		packetRecv >> nextX;
		packetRecv >> nextY;
		break;
	}

	sf::Uint16 stamina;
	packetRecv >> stamina;

	sf::Uint16 direction;
	packetRecv >> direction;

	artemis::Entity* character = zone_->GetServerEntity(serverId);
	if (character == nullptr)
	{
		SendCharacterRequest(serverId);
		return;
	}

	AttributeComponent* attributeComponent = attributeMapper.get(*character);
	PositionComponent* positionComponent = positionMapper.get(*character);
	if (attributeComponent != nullptr && positionComponent != nullptr)
	{
		if (result == Network::GameProtocol::SUCCESS)
		{
			attributeComponent->SetStamina(stamina);

			if (serverId != myServerId_)
			{
				attributeComponent->SetDirection(static_cast<unsigned char>(direction));

				switch (packetId)
				{
				case Network::GameProtocol::PKT_MOVEFIRST:
				case Network::GameProtocol::PKT_MOVEMIDDLE:
				case Network::GameProtocol::PKT_MOVEEND:
					movementSystem_->SetMove(
						*zone_->GetServerEntity(serverId),
						sf::Vector2i(firstX, firstY),
						sf::Vector2i(destinationX, destinationY));
					break;
				case Network::GameProtocol::PKT_RUN_MOVEFIRST:
				case Network::GameProtocol::PKT_RUN_MOVEMIDDLE:
				case Network::GameProtocol::PKT_RUN_MOVEEND:
					movementSystem_->SetRunMove(
						*zone_->GetServerEntity(serverId),
						sf::Vector2i(firstX, firstY),
						sf::Vector2i(destinationX, destinationY),
						sf::Vector2i(nextX, nextY));
					break;
				}
			}
		}
		else
		{
			MovementComponent* movementComponent = movementMapper.get(*character);
			if (movementComponent != nullptr)
			{
				if (serverId == myServerId_)
				{
					while (!movementComponent->GetQueue().empty()) movementComponent->GetQueue().pop();
				}
				movementComponent->SetStartCellPosition(positionComponent->GetCellPosition());
				movementComponent->SetNextCellPosition(positionComponent->GetCellPosition());
				movementComponent->SetSentNextRunCellPosition(sf::Vector2i(-1, -1));
			}

			positionComponent->SetCellPosition(sf::Vector2i(destinationX, destinationY));
			attributeComponent->SetState(StateType::Idle);
		}
	}
}

void PlayState::SendCharacterRequest(const unsigned int serverId)
{
	auto it = std::find(characterRequests_.cbegin(), characterRequests_.cend(), serverId);
	if (it == characterRequests_.cend())
	{
		characterRequests_.push_back(serverId);

		Network::GamePacket packet(Network::GameProtocol::PKT_USERMODIFY);
		packet << serverId;
		Game::Instance().gameSocket->Send(packet);
	}
}

void PlayState::HandleBattleMode(Network::GamePacket& packetRecv)
{
	sf::Int32 serverId;
	packetRecv >> serverId;

	sf::Uint8 battleMode;
	packetRecv >> battleMode;

	AttributeComponent* attributeComponent = attributeMapper.get(*zone_->GetServerEntity(serverId));
	if (attributeComponent != nullptr)
	{
		attributeComponent->SetBattleMode(battleMode);
	}
	else
	{
		SendCharacterRequest(serverId);
	}
}

void PlayState::AddChat(const std::string& chatMessage, const unsigned char chatType, const CEGUI::colour chatColor)
{
	using namespace Network::GameProtocol;
	switch (chatType)
	{
	case ChatType::NORMAL:
		chatHistoryWindow_->AddChat(chatMessage, chatType, chatColor);
		break;
	case ChatType::WHISPER:
	case ChatType::WHISPER_ME:
		break;
	case ChatType::ZONE:
	case ChatType::SYSTEM:
	case ChatType::GM_NOTICE:
	case ChatType::GUILD:
	case ChatType::SHOUT:
	case ChatType::YELLOW_STAT:
	case ChatType::BLUE_STAT:
	case ChatType::PARTY:
		chatHistoryWindow_->AddChat(chatMessage, chatType, chatColor);
		Game::Instance().gui->FireEvent("Chat/Frame", GUI::ChatWindow::EventChatAdded);
		break;
	case ChatType::GM_SCROLL_NOTICE:
		break;
	}
}

void PlayState::HandleChat(Network::GamePacket& packetRecv)
{
	using namespace Network::GameProtocol;

	sf::Uint8 chatType;
	packetRecv >> chatType;

	switch (chatType)
	{
	case ChatType::NORMAL:
	case ChatType::SHOUT:
	case ChatType::GUILD:
	case ChatType::ZONE:
	case ChatType::PARTY:
		{
			sf::Uint32 serverId;
			packetRecv >> serverId;

			sf::Uint16 classType = 0;
			if (chatType != ChatType::GUILD && chatType != ChatType::PARTY)
			{
				packetRecv >> classType;
			}

			sf::Uint8 r = 0;
			sf::Uint8 g = 0;
			sf::Uint8 b = 0;
			if (chatType == ChatType::ZONE)
			{
				packetRecv >> r;
				packetRecv >> g;
				packetRecv >> b;
			}

			std::string name;
			packetRecv >> name;
			boost::to_upper(name);

			std::string chat;
			packetRecv >> chat;
			
			if (!chat.empty())
			{
				if (chatType == ChatType::NORMAL || chatType == ChatType::SHOUT || chatType == ChatType::ZONE)
				{
					Entity* e = zone_->GetServerEntity(serverId);
					if (e)
					{
						ChatComponent* chatComponent = chatMapper.get(*e);
						if (chatComponent)
						{
							chatComponent->SetChat(chat);
							chatComponent->SetTime(0);
						}
					}
				}
			}
			if (serverId < 10000)
			{
				if (chatType == ChatType::ZONE)
				{
					CEGUI::colour chatColor(r / 255.f, g / 255.f, b / 255.f);
					AddChat(boost::str(boost::format("%1% : %2%") % name % chat), chatType, chatColor);
				}
				else
				{
					AddChat(boost::str(boost::format("%1% : %2%") % name % chat), chatType);
				}
			}
		}
		break;
	case ChatType::WHISPER: // Fall-through!
	case ChatType::WHISPER_ME:
		{
			sf::Uint16 classType = 0;
			packetRecv >> classType;

			bool isGameMaster = false;
			packetRecv >> isGameMaster;

			std::string name;
			packetRecv >> name;
			boost::to_upper(name);

			std::string chat;
			packetRecv >> chat;

			privateMessage_->Add(name, chat, classType, chatType == ChatType::WHISPER_ME);
		}
		break;
	case ChatType::SYSTEM:
	case ChatType::GM_NOTICE:
	case ChatType::YELLOW_STAT:
	case ChatType::BLUE_STAT:
		{
			std::string message;
			packetRecv >> message;
			AddChat(message, chatType);
		}
		break;
	case ChatType::GM_SCROLL_NOTICE:
		break;
	}
}

void PlayState::HandleCharacterData(Network::GamePacket& packetRecv)
{
	sf::Uint32 serverId;
	packetRecv >> serverId;

	AttributeComponent* attributeComponent = attributeMapper.get(*zone_->GetServerEntity(serverId));
	if (attributeComponent == nullptr)
	{
		return;
	}

	sf::Uint8 type;
	packetRecv >> type;

	if (type & Network::GameProtocol::INFO_NAMES)
	{
		std::string name;
		packetRecv >> name;

		sf::Int32 pos;
		packetRecv >> pos;

		std::string guildName;
		packetRecv >> guildName;

		std::string designation;
		packetRecv >> designation;

		attributeComponent->SetName(name);
		attributeComponent->SetGuildName(guildName);
		attributeComponent->SetPos(pos);
	}

	if (type & Network::GameProtocol::INFO_BASICVALUE)
	{
		sf::Uint32 money;
		packetRecv >> money;

		sf::Int16 moral;
		packetRecv >> moral;

		sf::Uint16 age;
		packetRecv >> age;

		sf::Uint16 gender;
		packetRecv >> gender;

		sf::Uint16 strength;
		packetRecv >> strength;

		sf::Uint16 dexterity;
		packetRecv >> dexterity;

		sf::Uint16 intelligence;
		packetRecv >> intelligence;

		sf::Uint16 wisdom;
		packetRecv >> wisdom;

		sf::Uint16 charisma;
		packetRecv >> charisma;

		sf::Uint16 constitution;
		packetRecv >> constitution;

		attributeComponent->SetMoral(moral);
		attributeComponent->SetMoney(money);
		attributeComponent->SetAge(age);
		attributeComponent->SetGender(gender == 0 ? Gender::Female : Gender::Male);
		attributeComponent->SetStr(strength);
		attributeComponent->SetDex(dexterity);
		attributeComponent->SetInt(intelligence);
		attributeComponent->SetWis(wisdom);
		attributeComponent->SetCha(charisma);
		attributeComponent->SetCon(constitution);
	}

	if (type & Network::GameProtocol::INFO_EXTVALUE)
	{
		sf::Uint16 level;
		packetRecv >> level;

		sf::Uint32 maximumExp;
		packetRecv >> maximumExp;

		sf::Uint32 exp;
		packetRecv >> exp;

		sf::Uint16 maximumHealth;
		packetRecv >> maximumHealth;

		sf::Uint16 health;
		packetRecv >> health;

		sf::Uint16 maximumMana;
		packetRecv >> maximumMana;

		sf::Uint16 mana;
		packetRecv >> mana;

		sf::Uint16 maximumWeight;
		packetRecv >> maximumWeight;

		sf::Uint16 weight;
		packetRecv >> weight;

		sf::Uint16 maximumStamina;
		packetRecv >> maximumStamina;

		sf::Uint16 stamina;
		packetRecv >> stamina;

		attributeComponent->SetLevel(level);
		attributeComponent->SetExp(exp);
		attributeComponent->SetMaximumExp(maximumExp);
		attributeComponent->SetHealth(health);
		attributeComponent->SetMana(mana);
		attributeComponent->SetStamina(stamina);
		attributeComponent->SetWeight(weight);
		attributeComponent->SetMaximumHealth(maximumHealth);
		attributeComponent->SetMaximumMana(maximumMana);
		attributeComponent->SetMaximumStamina(maximumStamina);
		attributeComponent->SetMaximumWeight(maximumWeight);
	}

	if (type & Network::GameProtocol::INFO_WEAPONEXP)
	{
		sf::Uint32 swordExp;
		packetRecv >> swordExp;

		sf::Uint32 spearExp;
		packetRecv >> spearExp;

		sf::Uint32 axeExp;
		packetRecv >> axeExp;

		sf::Uint32 knuckleExp;
		packetRecv >> knuckleExp;

		sf::Uint32 bowExp;
		packetRecv >> bowExp;

		sf::Uint32 staffExp;
		packetRecv >> staffExp;

		attributeComponent->SetSwordExp(swordExp);
		attributeComponent->SetSpearExp(spearExp);
		attributeComponent->SetAxeExp(axeExp);
		attributeComponent->SetKnuckleExp(knuckleExp);
		attributeComponent->SetBowExp(bowExp);
		attributeComponent->SetStaffExp(staffExp);
	}

	if (type & Network::GameProtocol::INFO_MAKEEXP)
	{
		sf::Uint32 weaponMakeExp;
		packetRecv >> weaponMakeExp;

		sf::Uint32 armorMakeExp;
		packetRecv >> armorMakeExp;

		sf::Uint32 accessoryMakeExp;
		packetRecv >> accessoryMakeExp;

		sf::Uint32 potionMakeExp;
		packetRecv >> potionMakeExp;

		sf::Uint32 cookingExp;
		packetRecv >> cookingExp;

		attributeComponent->SetWeaponMakeExp(weaponMakeExp);
		attributeComponent->SetArmorMakeExp(armorMakeExp);
		attributeComponent->SetAccessoryMakeExp(accessoryMakeExp);
		attributeComponent->SetPotionMakeExp(potionMakeExp);
		attributeComponent->SetCookingExp(cookingExp);
	}

	if (type & Network::GameProtocol::INFO_MAGICEXP)
	{
		sf::Uint32 blackMagicExp;
		packetRecv >> blackMagicExp;

		sf::Uint32 whiteMagicExp;
		packetRecv >> whiteMagicExp;

		sf::Uint32 blueMagicExp;
		packetRecv >> blueMagicExp;

		attributeComponent->SetBlackMagicExp(blackMagicExp);
		attributeComponent->SetWhiteMagicExp(whiteMagicExp);
		attributeComponent->SetBlueMagicExp(blueMagicExp);
	}
}

void PlayState::HandleHPMPRecovery(Network::GamePacket& packetRecv)
{
	sf::Uint32 serverId;
	packetRecv >> serverId;
	if (serverId != myServerId_)
	{
		return;
	}

	sf::Uint16 health;
	packetRecv >> health;

	sf::Uint16 mana;
	packetRecv >> mana;

	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent != nullptr)
	{
		attributeComponent->SetHealth(health);
		attributeComponent->SetMana(mana);
	}
}

void PlayState::HandleChangeDirection(Network::GamePacket& packetRecv)
{
	sf::Uint32 serverId;
	packetRecv >> serverId;

	AttributeComponent* attributeComponent = attributeMapper.get(*zone_->GetServerEntity(serverId));
	if (attributeComponent != nullptr)
	{
		sf::Uint8 direction;
		packetRecv >> direction;
		attributeComponent->SetDirection(direction);
	}
}

void PlayState::SendChangeDirection(const sf::Uint16 direction)
{
	Network::GamePacket packet(Network::GameProtocol::PKT_CHANGEDIR);
	packet << myServerId_ << direction;
	Game::Instance().gameSocket->Send(packet);
}

void PlayState::HandleInventoryAll(Network::GamePacket& packetRecv)
{
	sf::Int32 serverId;
	packetRecv >> serverId;

	InventoryComponent* inventoryComponent = inventoryMapper.get(*zone_->GetServerEntity(serverId));
	if (inventoryComponent == nullptr)
	{
		return;
	}

	sf::Uint16 inventoryCount;
	packetRecv >> inventoryCount;

	sf::Uint16 beltCount;
	packetRecv >> beltCount;

	inventoryComponent->ClearInventoryItem();
	inventoryComponent->ResizeInventoryItem(Network::GameProtocol::INV_ITEM_NUM);
	for (unsigned int i = 0; i < inventoryCount; ++i)
	{
		sf::Uint16 index;
		packetRecv >> index;
		inventoryComponent->GetInventoryItem(index).HandleItemReceive(packetRecv, Network::GameProtocol::ITEM_ME, false);
	}

	inventoryComponent->ClearBeltItem();
	inventoryComponent->ResizeBeltItem(Network::GameProtocol::BELT_ITEM_NUM);
	for (unsigned int i = 0; i < beltCount; ++i)
	{
		sf::Uint16 index;
		packetRecv >> index;
		inventoryComponent->GetBeltItem(index).HandleItemReceive(packetRecv, Network::GameProtocol::ITEM_ME, false);
	}

	engine_.gui.FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
	engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
	engine_.gui.FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
	engine_.gui.FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
	engine_.gui.FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
	engine_.gui.FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);

	engine_.gui.FireEvent("Hud/Frame", GUI::HUD::EventQuickSlotChanged);
}

void PlayState::HandleItemUse(Network::GamePacket& packetRecv)
{
	InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
	if (inventoryComponent == nullptr)
	{
		return;
	}

	sf::Uint8 result;
	packetRecv >> result;

	if (result != Network::GameProtocol::SUCCESS)
	{
		return;
	}

	sf::Uint8 beltOrInventory;
	packetRecv >> beltOrInventory;

	sf::Uint16 itemIndex;
	packetRecv >> itemIndex;

	sf::Uint8 itemType;
	packetRecv >> itemType;

	sf::Uint16 remainingDura;
	packetRecv >> remainingDura;

	sf::Int16 torchLightSeconds; // Used to set the length of the light given by a torch item.
	packetRecv >> torchLightSeconds;

	switch (beltOrInventory)
	{
	case 0: // Belt
		inventoryComponent->GetBeltItem(itemIndex).SetIndex(itemIndex);
		inventoryComponent->GetBeltItem(itemIndex).SetDura(remainingDura);
		engine_.gui.FireEvent("Hud/Frame", GUI::HUD::EventQuickSlotChanged);
		break;
	case 1: // Inventory
		inventoryComponent->GetInventoryItem(itemIndex).SetIndex(itemIndex);
		inventoryComponent->GetInventoryItem(itemIndex).SetDura(remainingDura);
		engine_.gui.FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
		engine_.gui.FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);
		break;
	}
}

void PlayState::HandleChangeBeltIndex(Network::GamePacket& packetRecv)
{
	InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
	if (inventoryComponent == nullptr)
	{
		return;
	}

	sf::Uint8 result;
	packetRecv >> result;

	if (result != Network::GameProtocol::SUCCESS)
	{
		return;
	}

	sf::Uint16 fromIndex;
	packetRecv >> fromIndex;

	sf::Uint16 toIndex;
	packetRecv >> toIndex;

	std::swap(inventoryComponent->GetBeltItem(fromIndex), inventoryComponent->GetBeltItem(toIndex));
	engine_.gui.FireEvent("Hud/Frame", GUI::HUD::EventQuickSlotChanged);
}

void PlayState::HandleChangeItemInfo(Network::GamePacket& packetRecv)
{
	InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
	if (inventoryComponent == nullptr)
	{
		return;
	}

	sf::Uint8 beltOrInventory;
	packetRecv >> beltOrInventory;

	sf::Int16 itemIndex;
	packetRecv >> itemIndex;

	sf::Uint8 type;
	packetRecv >> type;

	switch (beltOrInventory)
	{
	case 0: // Belt
		inventoryComponent->GetBeltItem(itemIndex).SetIndex(itemIndex);
		inventoryComponent->GetBeltItem(itemIndex).HandleChangeItemInfo(packetRecv, type);
		engine_.gui.FireEvent("Hud/Frame", GUI::HUD::EventQuickSlotChanged);
		break;
	case 1: // Inventory
		inventoryComponent->GetInventoryItem(itemIndex).SetIndex(itemIndex);
		inventoryComponent->GetInventoryItem(itemIndex).HandleChangeItemInfo(packetRecv, type);
		engine_.gui.FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
		engine_.gui.FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);
		break;
	default:
		std::cerr << "PlayState: HandleChangeItemInfo: beltOrInventory invalid value=" << beltOrInventory << std::endl;
	}
}

void PlayState::HandleItemZoneInfo(Network::GamePacket& packetRecv)
{
	sf::Uint16 itemCount;
	packetRecv >> itemCount;

	for (sf::Uint16 i = 0; i < itemCount; ++i)
	{
		sf::Uint8 type;
		packetRecv >> type;

		sf::Uint16 x;
		packetRecv >> x;

		sf::Uint16 y;
		packetRecv >> y;

		const sf::Vector2i itemCellPosition(x, y);

		artemis::Entity* itemToRemove = zone_->GetItemEntity(itemCellPosition);
		if (itemToRemove != nullptr) world_->deleteEntity(*itemToRemove);

		if (type == Network::GameProtocol::INFO_MODIFY)
		{
			sf::Uint16 pictureId;
			packetRecv >> pictureId;

			sf::Uint8 itemType;
			packetRecv >> itemType;

			sf::Uint32 dura;
			packetRecv >> dura;

			std::string name;
			packetRecv >> name;

			artemis::Entity& item = EntityFactory::CreateItem(*world_, itemCellPosition);
			ItemAttributeComponent* itemAttributeComponent = itemAttributeMapper.get(item);
			assert(itemAttributeComponent != nullptr && "Entity is invalid (must have item attribute component)");
			itemAttributeComponent->SetPictureId(pictureId);
			itemAttributeComponent->SetType(itemType);
			itemAttributeComponent->SetDura(dura);
			itemAttributeComponent->SetName(name);
			zone_->AddItemEntity(itemCellPosition, item);
		}
		else if (type == Network::GameProtocol::INFO_DELETE)
		{
			zone_->RemoveItemEntity(itemCellPosition);
		}
	}
}

void PlayState::HandleItemThrow(Network::GamePacket& packetRecv)
{
	sf::Uint8 result;
	packetRecv >> result;

	if (result != Network::GameProtocol::SUCCESS)
	{
		return;
	}

	sf::Uint8 beltOrInventory;
	packetRecv >> beltOrInventory;

	sf::Uint16 itemIndex;
	packetRecv >> itemIndex;

	sf::Uint16 remainingDura;
	packetRecv >> remainingDura;

	InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
	if (inventoryComponent == nullptr)
	{
		return;
	}

	switch (beltOrInventory)
	{
	case 0: // Belt
		inventoryComponent->GetBeltItem(itemIndex).SetIndex(itemIndex);
		inventoryComponent->GetBeltItem(itemIndex).SetDura(remainingDura);
		engine_.gui.FireEvent("Hud/Frame", GUI::HUD::EventQuickSlotChanged);
		break;
	case 1: // Inventory
		inventoryComponent->GetInventoryItem(itemIndex).SetIndex(itemIndex);
		inventoryComponent->GetInventoryItem(itemIndex).SetDura(remainingDura);
		engine_.gui.FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
		engine_.gui.FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);
		break;
	default:
		std::cerr << "PlayState: HandleItemThrow: beltOrInventory invalid value=" << beltOrInventory << std::endl;
	}
}

void PlayState::HandleItemPickup(Network::GamePacket& packetRecv)
{
	sf::Uint8 result;
	packetRecv >> result;

	if (result != Network::GameProtocol::SUCCESS)
	{
		return;
	}

	sf::Uint8 beltOrInventory;
	packetRecv >> beltOrInventory;

	sf::Uint16 itemIndex;
	packetRecv >> itemIndex;

	InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
	if (inventoryComponent == nullptr)
	{
		return;
	}

	switch (beltOrInventory)
	{
	case 0: // Belt
		inventoryComponent->GetBeltItem(itemIndex).HandleItemReceive(packetRecv, Network::GameProtocol::ITEM_ME, false);
		engine_.gui.FireEvent("Hud/Frame", GUI::HUD::EventQuickSlotChanged);
		break;
	case 1: // Inventory
		inventoryComponent->GetInventoryItem(itemIndex).HandleItemReceive(packetRecv, Network::GameProtocol::ITEM_ME, false);
		engine_.gui.FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
		engine_.gui.FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);
		break;
	default:
		std::cerr << "PlayState: HandleItemPickup: beltOrInventory invalid value=" << beltOrInventory << std::endl;
	}
}

void PlayState::HandleMoneyChange(Network::GamePacket& packetRecv)
{
	sf::Uint32 money;
	packetRecv >> money;

	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent != nullptr)
	{
		attributeComponent->SetMoney(money);
		// TODO: More fine grained control over what gets updated. So could update the money only.
		engine_.gui.FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
		engine_.gui.FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);
	}
}

void PlayState::HandleWeightChange(Network::GamePacket& packetRecv)
{
	sf::Uint16 weight;
	packetRecv >> weight;

	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent!= nullptr)
	{
		attributeComponent->SetWeight(weight);
	}
}

void PlayState::HandleMagicAll(Network::GamePacket& packetRecv)
{
	sf::Uint8 type;
	packetRecv >> type;

	sf::Uint16 count;
	packetRecv >> count;

	InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
	if (inventoryComponent == nullptr)
	{
		return;
	}

	switch (type)
	{
	case 1: // Magic
		inventoryComponent->ClearMagic();
		for (unsigned int i = 0; i < count; ++i)
		{
			inventoryComponent->AddMagic(packetRecv);
		}
		break;
	case 2: // Special Attack
		inventoryComponent->ClearSpecialAttack();
		for (unsigned int i = 0; i < count; ++i)
		{
			inventoryComponent->AddSpecialAttack(packetRecv);
		}
		break;
	case 3: // Ability
		inventoryComponent->ClearAbility();
		for (unsigned int i = 0; i < count; ++i)
		{
			inventoryComponent->AddAbility(packetRecv);
		}

		sf::Uint16 skipCount;
		packetRecv >> skipCount;
		for (unsigned int i = 0; i < skipCount; ++i)
		{
			sf::Uint16 temp;
			packetRecv >> temp;
		}

		hud_->HandleSetMagicSlots(packetRecv);
		break;
	default:
		break;
	}
}

void PlayState::HandleEvents()
{
	static bool focused = true;

	static bool guiHandledMouseEvent = false;

	bool leftMouseButtonPressed = false;
	bool rightMouseButtonPressed = false;
	bool leftMouseButtonReleased = false;
	bool rightMouseButtonReleased = false;

    sf::Event e;
    while (engine_.renderWindow.pollEvent(e))
    {
		if (!engine_.gui.HandleEvent(engine_.renderWindow, e))
		{
			switch (e.type)
			{
			case sf::Event::Closed:
				SendLogout(false);
				engine_.Quit();
				break;
			case sf::Event::GainedFocus:
				focused = true;
				break;
			case sf::Event::LostFocus:
				focused = false;
				break;
			case sf::Event::KeyPressed:
				HandleKeyPressed(e);
				break;
			case sf::Event::MouseButtonPressed:
				switch (e.mouseButton.button)
				{
				case sf::Mouse::Left:
					leftMouseButtonPressed = true;
					break;
				case sf::Mouse::Right:
					rightMouseButtonPressed = true;
					break;
				}
				guiHandledMouseEvent = false;
				break;
			case sf::Event::MouseButtonReleased:
				switch (e.mouseButton.button)
				{
				case sf::Mouse::Left:
					leftMouseButtonReleased = true;
					break;
				case sf::Mouse::Right:
					rightMouseButtonReleased = true;
					break;
				}
				guiHandledMouseEvent = false;
				break;
			}
		}
		else
		{
			if (e.type == sf::Event::MouseButtonPressed || e.type == sf::Event::MouseButtonReleased)
			{
				guiHandledMouseEvent = true;
			}
		}
	}

	if (!guiHandledMouseEvent)
	{
		if (leftMouseButtonPressed)
		{
			HandleLeftMouseButtonPressed(e);
		}
		else if (leftMouseButtonReleased)
		{
			HandleLeftMouseButtonReleased(e);
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && focused)
		{
			HandleLeftMouseButtonPressing(e);
		}

		if (rightMouseButtonPressed)
		{
			HandleRightMouseButtonPressed(e);
		}
		else if (rightMouseButtonReleased)
		{
			HandleRightMouseButtonReleased(e);
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && focused)
		{
			HandleRightMouseButtonPressing(e);
		}
	}
}

void PlayState::HandleKeyPressed(const sf::Event& e)
{
	switch (e.key.code)
	{
	case sf::Keyboard::Q:
		if (e.key.control)
		{
			engine_.ChangeState(boost::make_shared<SelectCharacterState>(engine_));
			SendLogout(true);
		}
		break;
	case sf::Keyboard::P:
		if (e.key.control)
		{
			SendPartyDeny();
		}
		break;
	case sf::Keyboard::B:
		if (e.key.control)
		{
			engine_.renderWindow.capture().saveToFile("screenshot.png");
		}
		break;
	case sf::Keyboard::Numpad0:
		renderToggle[0] = !renderToggle[0];
		break;
	case sf::Keyboard::Numpad1:
		renderToggle[1] = !renderToggle[1];
		break;
	case sf::Keyboard::Numpad2:
		renderToggle[2] = !renderToggle[2];
		break;
	case sf::Keyboard::Numpad3:
		renderToggle[3] = !renderToggle[3];
		break;
	}
}

void PlayState::HandleLeftMouseButtonPressed(const sf::Event& e)
{
	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent == nullptr || !attributeComponent->IsAlive())
	{
		return;
	}

	characterClickedServerId_ = focusSystem_->GetCharacterFocusServerId();

	if (!HandleCharacterClicked(false))
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || 
			sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
		{
			RotateMyCharacter();
		}
		else
		{
			MoveMyCharacter();
		}
	}
}

void PlayState::HandleLeftMouseButtonPressing(const sf::Event& e)
{
	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent == nullptr || !attributeComponent->IsAlive())
	{
		return;
	}

	if (!HandleCharacterClicked(true))
	{
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) &&
			(attributeComponent->GetState() == StateType::Walking || attributeComponent->GetState() == StateType::Running))
		{
			MoveMyCharacter();
		}
	}
}

void PlayState::HandleLeftMouseButtonReleased(const sf::Event& e)
{
	characterClickedServerId_ = -1;
	focusSystem_->SetCharacterAttackServerId(-1);
}

void PlayState::HandleRightMouseButtonPressed(const sf::Event& e)
{
	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent == nullptr || !attributeComponent->IsAlive())
	{
		return;
	}

	if (attributeComponent->GetBattleMode() == BattleMode::Normal)
	{
		const int focusServerId = focusSystem_->GetCharacterFocusServerId();
		if (focusServerId != -1 && focusServerId < 10000 && focusServerId != myServerId_)
		{
			PositionComponent* positionComponent = positionMapper.get(*Game::Instance().myEntity);
			PositionComponent* focusPositionComponent = positionMapper.get(*zone_->GetServerEntity(focusServerId));
			if (positionComponent && focusPositionComponent)
			{
				if (abs(positionComponent->GetCellPosition().x - focusPositionComponent->GetCellPosition().x) +
					abs(positionComponent->GetCellPosition().y - focusPositionComponent->GetCellPosition().y) <= 2)
				{
					exchangeWindow_->HandleAskTrade(*zone_->GetServerEntity(focusServerId));
				}
			}
		}
	}
	else
	{
		magicSystem_->PlayerMagicCast(hud_->GetSelectedMagicId());
	}
}

void PlayState::HandleRightMouseButtonPressing(const sf::Event& e)
{
	AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	if (attributeComponent == nullptr || !attributeComponent->IsAlive())
	{
		return;
	}

	if (attributeComponent->GetBattleMode() != BattleMode::Normal)
	{
		magicSystem_->PlayerMagicCast(hud_->GetSelectedMagicId());
	}
}

void PlayState::HandleRightMouseButtonReleased(const sf::Event& e)
{

}

void PlayState::RotateMyCharacter()
{
	PositionComponent* positionComponent = positionMapper.get(*Game::Instance().myEntity);
	if (positionComponent)
	{
		sf::Vector2i mousePosition(Utils::PositionConvert::PixelToCell(engine_.renderWindow.mapPixelToCoords(
			sf::Mouse::getPosition(engine_.renderWindow),
			cameraSystem_->GetCamera())));

		SendChangeDirection(MovementSystem::GetDirection(positionComponent->GetCellPosition(), mousePosition));
	}
}

void PlayState::MoveMyCharacter()
{
	PositionComponent* positionComponent = positionMapper.get(*Game::Instance().myEntity);
	MovementComponent* movementComponent = movementMapper.get(*Game::Instance().myEntity);
	if (positionComponent == nullptr || movementComponent == nullptr)
	{
		return;
	}

	sf::Vector2i mouseCellPosition(Utils::PositionConvert::PixelToCell(engine_.renderWindow.mapPixelToCoords(
		sf::Mouse::getPosition(engine_.renderWindow),
		cameraSystem_->GetCamera())));

	if (movementComponent->GetNextCellPosition().x == -1 && movementComponent->GetNextCellPosition().y == -1)
	{
		movementComponent->SetNextCellPosition(positionComponent->GetCellPosition());
	}

	sf::Vector2i startCellPosition = movementComponent->GetNextCellPosition();
	sf::Vector2i endCellPosition = mouseCellPosition;
	if (movementComponent->GetSentNextRunCellPosition().x != -1 &&
		movementComponent->GetSentNextRunCellPosition().y != -1)
	{
		startCellPosition = movementComponent->GetSentNextRunCellPosition();
	}

	if (startCellPosition.x == endCellPosition.x &&
		startCellPosition.y == endCellPosition.y)
	{
		return;
	}

	while (!movementComponent->GetQueue().empty()) movementComponent->GetQueue().pop();

	std::list<std::pair<int, int>> path = pathFind_->FindPath(*zone_.get(), 
		startCellPosition.x, startCellPosition.y,
		mouseCellPosition.x, mouseCellPosition.y);

	if (!path.empty())
	{
		for (auto it = path.cbegin(); it != path.cend(); ++it)
		{
			movementComponent->GetQueue().push(sf::Vector2i((*it).first, (*it).second));
		}
		movementComponent->SetDestinationPosition(Utils::PositionConvert::CellToPixel(
			sf::Vector2i(path.back().first, path.back().second)));

		AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
		if (attributeComponent->GetState() != StateType::Walking && attributeComponent->GetState() != StateType::Running)
		{
			attributeComponent->GetRunToggle()
				? attributeComponent->SetState(StateType::Running)
				: attributeComponent->SetState(StateType::Walking);
		}
		movementComponent->SetRunning(attributeComponent->GetRunToggle());
	}
}

bool PlayState::HandleCharacterClicked(const bool mousePressing)
{
	if (characterClickedServerId_ == -1 || myServerId_ == characterClickedServerId_)
	{
		return false;
	}

	AttributeComponent* attributeComponent = attributeMapper.get(*zone_->GetServerEntity(characterClickedServerId_));
	if (attributeComponent == nullptr)
	{
		characterClickedServerId_ = -1;
		return false;
	}

	return attributeComponent->IsAttackable()
		? AttackCharacter()
		: TalkToCharacter(characterClickedServerId_, mousePressing);
}

bool PlayState::AttackCharacter()
{
	AttributeComponent* myAttributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	PositionComponent* myPositionComponent = positionMapper.get(*Game::Instance().myEntity);
	if (myAttributeComponent == nullptr || myPositionComponent == nullptr || !myAttributeComponent->IsAlive())
	{
		characterClickedServerId_ = -1;
		focusSystem_->SetCharacterAttackServerId(-1);
		return false;
	}

	// A valid handled state for when the attack animation is still running because of a previous attack.
	if (myAttributeComponent->GetState() == StateType::Attacking)
	{
		return true;
	}

	// Prevent sending of attack if have not yet received a result for a previous attack
	// and the time gap is < 3 seconds.
	if (attackFlag_ && attackTimer_.getElapsedTime().asMilliseconds() < 3000)
	{
		return true;
	}

	AttributeComponent* targetAttributeComponent = attributeMapper.get(*zone_->GetServerEntity(characterClickedServerId_));
	PositionComponent* targetPositionComponent = positionMapper.get(*zone_->GetServerEntity(characterClickedServerId_));
	if (targetAttributeComponent == nullptr || targetPositionComponent == nullptr || !targetAttributeComponent->IsAlive())
	{
		characterClickedServerId_ = -1;
		focusSystem_->SetCharacterAttackServerId(-1);
		return false;
	}

	bool canAttack = false;
	BattleMode::Enum myBattleMode = myAttributeComponent->GetBattleMode();
	if ((myBattleMode == BattleMode::Battle && characterClickedServerId_ >= 10000) || myBattleMode == BattleMode::PK)
	{
		canAttack = true;
	}

	bool attacked = false;

	if (canAttack)
	{
		int distance = 2;
		InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
		if (inventoryComponent != nullptr)
		{
			const ItemAttributeComponent& item = inventoryComponent->GetInventoryItem(ItemEquipPosition::RightHand);
			if (!item.Empty())
			{
				if (item.GetType() == ItemType::Bow)
				{
					distance = 18;
				}
				else if (item.GetType() == ItemType::Spear)
				{
					distance = 4;
				}
			}
		}

		if (abs(myPositionComponent->GetCellPosition().x - targetPositionComponent->GetCellPosition().x) +
			abs(myPositionComponent->GetCellPosition().y - targetPositionComponent->GetCellPosition().y) <= distance)
		{
			MovementComponent* movementComponent = movementMapper.get(*Game::Instance().myEntity);
			if (movementComponent != nullptr)
			{
				while (!movementComponent->GetQueue().empty()) movementComponent->GetQueue().pop();
			}

			myAttributeComponent->SetState(StateType::Idle);

			Network::GamePacket packet(Network::GameProtocol::PKT_ATTACK);
			packet << characterClickedServerId_
				   << static_cast<short>(MovementSystem::GetDirection(myPositionComponent->GetCellPosition(),
				   targetPositionComponent->GetCellPosition()));
			Game::Instance().gameSocket->Send(packet);

			attackFlag_ = true;
			attackTimer_.restart();

			attacked = true;

			focusSystem_->SetCharacterAttackServerId(characterClickedServerId_);
		}
		else
		{
			characterClickedServerId_ = -1;
			focusSystem_->SetCharacterAttackServerId(-1);
		}
	}

	return attacked;
}

bool PlayState::TalkToCharacter(const int serverId, const bool mousePressing)
{ 
	if (serverId < 10000)
	{
		characterClickedServerId_ = -1;
		return false;
	}

	AttributeComponent* myAttributeComponent = attributeMapper.get(*Game::Instance().myEntity);
	PositionComponent* myPositionComponent = positionMapper.get(*Game::Instance().myEntity);
	if (myAttributeComponent == nullptr || myPositionComponent == nullptr || !myAttributeComponent->IsAlive())
	{
		characterClickedServerId_ = -1;
		return false;
	}

	AttributeComponent* targetAttributeComponent = attributeMapper.get(*zone_->GetServerEntity(serverId));
	PositionComponent* targetPositionComponent = positionMapper.get(*zone_->GetServerEntity(serverId));
	if (targetAttributeComponent == nullptr || targetPositionComponent == nullptr || !targetAttributeComponent->IsAlive())
	{
		characterClickedServerId_ = -1;
		return false;
	}

	bool talkedTo = false;

	// TODO: Add a utility function for manhattan distance?
	if (abs(myPositionComponent->GetCellPosition().x - targetPositionComponent->GetCellPosition().x) +
		abs(myPositionComponent->GetCellPosition().y - targetPositionComponent->GetCellPosition().y) <= 6)
	{
		MovementComponent* movementComponent = movementMapper.get(*Game::Instance().myEntity);
		if (movementComponent != nullptr)
		{
			while (!movementComponent->GetQueue().empty()) movementComponent->GetQueue().pop();
		}

		myAttributeComponent->SetState(StateType::Idle);
		
		if (!mousePressing)
		{
			Network::GamePacket packet(Network::GameProtocol::PKT_CLIENTEVENT);
			packet << static_cast<unsigned char>(2) // Type of event
				<< serverId;
			Game::Instance().gameSocket->Send(packet);
			eventClickPosition_ = sf::Mouse::getPosition(engine_.renderWindow);
		}

		talkedTo = true;
	}

	return talkedTo;
}

void PlayState::HandleAddItem(Network::GamePacket& packetRecv)
{
	InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
	if (inventoryComponent != nullptr)
	{
		sf::Uint16 index;
		packetRecv >> index;
		inventoryComponent->GetInventoryItem(index).HandleItemReceive(packetRecv, Network::GameProtocol::ITEM_ME, false);
		engine_.gui.FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
		engine_.gui.FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);
	}
}


void PlayState::HandleRemoveItem(Network::GamePacket& packetRecv)
{
	InventoryComponent* inventoryComponent = inventoryMapper.get(*Game::Instance().myEntity);
	if (inventoryComponent == nullptr)
	{
		return;
	}

	sf::Uint8 beltOrInventory;
	packetRecv >> beltOrInventory;

	sf::Uint16 itemIndex;
	packetRecv >> itemIndex;

	switch (beltOrInventory)
	{
	case 0: // Belt
		inventoryComponent->GetBeltItem(itemIndex).SetIndex(itemIndex);
		inventoryComponent->GetBeltItem(itemIndex).SetDura(0);
		engine_.gui.FireEvent("Hud/Frame", GUI::HUD::EventQuickSlotChanged);
		break;
	case 1: // Inventory
		inventoryComponent->GetInventoryItem(itemIndex).SetIndex(itemIndex);
		inventoryComponent->GetInventoryItem(itemIndex).SetDura(0);
		engine_.gui.FireEvent("Inventory/Frame", GUI::InventoryWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Shop/Frame", GUI::ShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("PosShop/Frame", GUI::PosShopWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Repair/Frame", GUI::RepairWindow::EventInventoryChanged);
		engine_.gui.FireEvent("Stash/Frame", GUI::StashWindow::EventInventoryChanged);
		engine_.gui.FireEvent("GuildStash/Frame", GUI::GuildStashWindow::EventInventoryChanged);
		break;
	}
}

void PlayState::HandleEventMessage(Network::GamePacket& packetRecv)
{
	CEGUI::System::getSingleton().getGUISheet()->getChild("EventMessageWindow/Frame")->hide();

	sf::Uint16 length;
	packetRecv >> length;

	std::string message;
	packetRecv.RetrieveString(message, length);

	eventMessageWindow_->AddMessage(message);
}

void PlayState::HandleEventSelection(Network::GamePacket& packetRecv)
{
	CEGUI::System::getSingleton().getGUISheet()->getChild("EventSelectionWindow/Frame")->hide();

	sf::Uint16 length;
	packetRecv >> length;

	std::string message;
	packetRecv.RetrieveString(message, length);

	eventSelectionWindow_->SetMessage(message);

	for (int i = 0; i < GUI::EventSelectionWindow::SELECTION_COUNT; ++i)
	{
		sf::Uint16 length;
		packetRecv >> length;

		std::string selection;
		packetRecv.RetrieveString(selection, length);
		eventSelectionWindow_->SetSelection(selection, i);
	}

	CEGUI::System::getSingleton().getGUISheet()->getChild("EventSelectionWindow/Frame")->show();
}

void PlayState::HandleEventMessageBoxOK(Network::GamePacket& packetRecv)
{
	sf::Uint16 length;
	packetRecv >> length;

	std::string message;
	packetRecv.RetrieveString(message, length);

	Game::Instance().messageBoxWindow->Show(message);
}

void PlayState::HandleEquipmentItemInfo(Network::GamePacket& packetRecv)
{
	sf::Int32 serverId;
	packetRecv >> serverId;

	EquipmentComponent* equipmentComponent = equipmentMapper.get(*zone_->GetServerEntity(serverId));
	if (equipmentComponent == nullptr)
	{
		return;
	}

	sf::Uint16 i;
	packetRecv >> i;

	sf::Int16 pictureId;
	packetRecv >> pictureId;
	if (pictureId == -1) pictureId = 0;

	sf::Uint16 attackDelay;
	packetRecv >> attackDelay;

	switch (i)
	{
	case ItemEquipPosition::Head:
		equipmentComponent->pictureId[ArmatureType::HelmetOrHair] = pictureId;
		break;
	case ItemEquipPosition::Chest:
		equipmentComponent->pictureId[ArmatureType::Top] = pictureId;
		break;
	case ItemEquipPosition::Leg:
		equipmentComponent->pictureId[ArmatureType::Pad] = pictureId;
		break;
	case ItemEquipPosition::Foot:
		equipmentComponent->pictureId[ArmatureType::Boot] = pictureId;
		break;
	case ItemEquipPosition::LeftHand:
		equipmentComponent->pictureId[ArmatureType::Shield] = pictureId;
		break;
	case ItemEquipPosition::RightHand:
		equipmentComponent->pictureId[ArmatureType::Weapon] = pictureId;
		break;
	}
}

void PlayState::HandleAttack(Network::GamePacket& packetRecv)
{
	sf::Uint8 result;
	packetRecv >> result;

	sf::Int32 serverId;
	packetRecv >> serverId;

	if (serverId == myServerId_)
	{
		attackFlag_ = false;
	}

	sf::Int32 targetServerId;
	packetRecv >> targetServerId;

	sf::Uint8 arrow;
	packetRecv >> arrow;

	// Direction is not used (also has wrong values) but still reading it from the packet.
	sf::Uint16 direction;
	packetRecv >> direction;

	// Skip over item dura change as will be receiving this information in another packet.
	// It needs to be removed server side.
	for (int i = 0; i < 4; ++i)
	{
		sf::Uint16 index;
		packetRecv >> index;

		sf::Uint16 dura;
		packetRecv >> dura;
	}

	if (result == Network::GameProtocol::ATTACK_FAIL)
	{
		return;
	}

	Entity* source = zone_->GetServerEntity(serverId);
	Entity* target = zone_->GetServerEntity(targetServerId);
	if (source == nullptr || target == nullptr)
	{
		return;
	}

	AttributeComponent* sourceAttributeComponent = attributeMapper.get(*source);
	AttributeComponent* targetAttributeComponent = attributeMapper.get(*target);
	if (sourceAttributeComponent == nullptr || targetAttributeComponent == nullptr)
	{
		return;
	}

	if (result == Network::GameProtocol::ATTACK_SUCCESS)
	{
		sf::Uint16 health;
		packetRecv >> health;

		sf::Uint16 maximumHealth;
		packetRecv >> maximumHealth;

		sf::Uint32 exp;
		packetRecv >> exp;

		sourceAttributeComponent->SetExp(exp);

		targetAttributeComponent->SetHealth(health);
		targetAttributeComponent->SetMaximumHealth(maximumHealth);
	}

	PositionComponent* sourcePositionComponent = positionMapper.get(*source);
	PositionComponent* targetPositionComponent = positionMapper.get(*target);
	if (sourcePositionComponent != nullptr && targetPositionComponent != nullptr)
	{
		if (serverId != myServerId_)
		{
			MovementComponent* sourceMovementComponent = movementMapper.get(*source);
			if (sourceMovementComponent != nullptr)
			{
				while (!sourceMovementComponent->GetQueue().empty())
				{
					zone_->SetMovable(sourceMovementComponent->GetQueue().front(), true);
					sourceMovementComponent->GetQueue().pop();
				}
				zone_->SetMovable(sourcePositionComponent->GetCellPosition(), false);
			}
		}
		// Direction is set using the positions of both characters involved. The direction value in packet
		// cannot be used as it is wrong and therefore not of any use.
		sourceAttributeComponent->SetDirection(MovementSystem::GetDirection(
			sourcePositionComponent->GetCellPosition(), targetPositionComponent->GetCellPosition()));
	}
	sourceAttributeComponent->SetState(StateType::Attacking);

	bool addArrow = false;

	EquipmentComponent* equipmentComponent = equipmentMapper.get(*source);
	if (equipmentComponent != nullptr)
	{
		const ItemTable::ItemElement* itemElement =
			Game::Instance().itemTable->Get(equipmentComponent->pictureId[ArmatureType::Weapon]);
		if (itemElement != nullptr)
		{
			if (itemElement->equipPosition == WeaponType::Bow || itemElement->equipPosition == WeaponType::CrossBow ||
				(serverId > 10000 && arrow > 0))
			{
				addArrow = true;
			}
		}
	}

	if (addArrow)
	{
		const bool hit = result == Network::GameProtocol::ATTACK_SUCCESS ? true : false;
		if (arrow <= 0) arrow = 1;
		EntityFactory::CreateArrow(*world_, *source, *target, arrow, hit);
	}

	if (result == Network::GameProtocol::ATTACK_SUCCESS && !addArrow)
	{
		targetAttributeComponent->SetState(StateType::Hit);
		EntityFactory::CreateBlood(*world_, *target, targetAttributeComponent->GetBloodType(), true);
	}
}

void PlayState::HandleDead(Network::GamePacket& packetRecv)
{
	sf::Int32 serverId;
	packetRecv >> serverId;

	sf::Uint16 x;
	packetRecv >> x;

	sf::Uint16 y;
	packetRecv >> y;

	Entity* entity = zone_->GetServerEntity(serverId);
	if (entity == nullptr)
	{
		return;
	}

	if (serverId != myServerId_)
	{
		zone_->SetMovable(sf::Vector2i(x, y), true);
	}
	else
	{
		grayscaleAlpha_ = 0;
		grayscaleFadeTimer_.restart();
		grayscaleShader_.setParameter("alpha", 0.0f);
		hud_->Hide();
		hudDead_->Show();
	}

	AttributeComponent* attributeComponent = attributeMapper.get(*entity);
	if (attributeComponent != nullptr)
	{
		attributeComponent->SetState(StateType::Idle);
		attributeComponent->SetAlive(false);
		attributeComponent->SetGrayMode(0);

		EntityFactory::CreateBlood(*world_, *entity, attributeComponent->GetBloodType(), false);
		entity->addComponent(new DeadComponent(0.4f));
		entity->refresh();
	}
}

void PlayState::HandleZoneChange(Network::GamePacket& packetRecv)
{
	sf::Int32 serverId;
	packetRecv >> serverId;

	sf::Uint16 x;
	packetRecv >> x;

	sf::Uint16 y;
	packetRecv >> y;

	sf::Uint16 zone;
	packetRecv >> zone;

	zone_->Load(zone);

	PositionComponent* positionComponent = positionMapper.get(*zone_->GetServerEntity(myServerId_));
	MovementComponent* movementComponent = movementMapper.get(*zone_->GetServerEntity(myServerId_));
	if (positionComponent != nullptr && movementComponent != nullptr)
	{
		positionComponent->SetCellPosition(sf::Vector2i(x, y));
		while (!movementComponent->GetQueue().empty()) movementComponent->GetQueue().pop();
		movementComponent->SetStartCellPosition(positionComponent->GetCellPosition());
		movementComponent->SetNextCellPosition(positionComponent->GetCellPosition());
	}

	// Remove all server entity except for the player from the zone and delete the entity from world.
	std::map<unsigned int, artemis::Entity*>& entityMap = zone_->GetServerEntityMap();
	auto it = entityMap.begin();
	while (it != entityMap.end())
	{
		if (it->first != myServerId_)
		{
			if (it->second != nullptr)
			{
				world_->deleteEntity(*it->second);
			}
			entityMap.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void PlayState::HandleTownPortalReq(Network::GamePacket& packetRecv)
{
	sf::Int32 serverId;
	packetRecv >> serverId;

	Network::GamePacket packet(Network::GameProtocol::PKT_TOWNPORTALREQ);
	packet << static_cast<sf::Int32>(serverId);
	Game::Instance().gameSocket->Send(packet);
}

void PlayState::HandleTownPortalEnd(Network::GamePacket& packetRecv)
{
	sf::Int32 serverId;
	packetRecv >> serverId;

	Network::GamePacket packet(Network::GameProtocol::PKT_TOWNPORTALEND);
	packet << static_cast<sf::Int32>(serverId);
	Game::Instance().gameSocket->Send(packet);
}

void PlayState::HandleMagicReady(Network::GamePacket& packetRecv)
{
	sf::Uint8 result;
	packetRecv >> result;

	sf::Int32 serverId;
	packetRecv >> serverId;

	sf::Int32 targetServerId;
	packetRecv >> targetServerId;

	sf::Uint16 direction;
	packetRecv >> direction;

	sf::Uint16 magicId;
	packetRecv >> magicId;

	sf::Uint8 distance;
	packetRecv >> distance;

	sf::Uint32 startTime;
	packetRecv >> startTime;

	if (result == Network::GameProtocol::FAIL)
	{
		return;
	}

	Entity* source = zone_->GetServerEntity(serverId);
	Entity* target = zone_->GetServerEntity(targetServerId);
	if (source == nullptr || target == nullptr)
	{
		return;
	}

	const MagicTable::MagicElement magicElement = *Game::Instance().magicTable->Get(magicId);
	Entity& magic = EntityFactory::CreateMagic(*world_, *source, magicElement);
	std::cout << "Magic Entity Added" << std::endl;
	MagicComponent* magicComponent = magicMapper.get(magic);
	magicComponent->GetMagicTable().startTime = startTime;
	magicComponent->SetDirection(static_cast<unsigned char>(direction));
	magicComponent->GetTargets().push_back(target);

	AttributeComponent* attributeComponent = attributeMapper.get(*source);
	attributeComponent->SetDirection(static_cast<unsigned char>(direction));
	attributeComponent->GetMagicEntityMap().insert(std::pair<unsigned short, Entity*>(magicId, &magic));

	std::cout << "HandleMagicReady" << std::endl;
}

void PlayState::HandleMagicArrow(Network::GamePacket& packetRecv)
{
	sf::Uint8 result;
	packetRecv >> result;

	sf::Uint16 direction;
	packetRecv >> direction;

	sf::Int32 serverId;
	packetRecv >> serverId;

	sf::Uint16 health;
	packetRecv >> health;

	sf::Uint16 mana;
	packetRecv >> mana;

	sf::Uint32 exp;
	packetRecv >> exp;

	sf::Int32 targetServerId;
	packetRecv >> targetServerId;

	sf::Uint16 magicId;
	packetRecv >> magicId;

	sf::Uint16 targetHealth;
	packetRecv >> targetHealth;

	sf::Uint16 targetMana;
	packetRecv >> targetMana;

	sf::Uint32 continueTime = 0;
	if (result != Network::GameProtocol::FAIL)
	{
		packetRecv >> continueTime;
	}

	Entity* source = zone_->GetServerEntity(serverId);
	if (source == nullptr)
	{
		return;
	}

	AttributeComponent* attributeComponent = attributeMapper.get(*source);
	if (attributeComponent == nullptr)
	{
		return;
	}

	//attributeComponent->SetDirection(direction);
	attributeComponent->SetHealth(health);
	attributeComponent->SetMana(mana);
	attributeComponent->SetExp(exp);

	Entity* magic = magicSystem_->GetMagicEntity(*source, magicId);
	if (magic != nullptr)
	{
		std::cout << "HandleMagicArrow1 magicId=" << magicId << std::endl;
		MagicComponent* magicComponent = magicMapper.get(*magic);
		if (!magicComponent->IsReady() && result == Network::GameProtocol::FAIL)
		{
			world_->deleteEntity(*magic);
			return;
		}

		magicComponent->GetMagicTable().continueTime = continueTime;
		magicComponent->SetReady(true);
		magicComponent->GetTargets().clear();

		Entity* target = zone_->GetServerEntity(targetServerId);
		if (target != nullptr)
		{
			AttributeComponent* targetAttributeComponent = attributeMapper.get(*target);
			if (targetAttributeComponent != nullptr)
			{
				targetAttributeComponent->SetHealth(targetHealth);
				targetAttributeComponent->SetMana(targetMana);
			}
			magicComponent->GetTargets().push_back(target);
		}
	}
	else
	{
		std::cout << "HandleMagicArrow2 magicId=" << magicId << std::endl;
		if (result == Network::GameProtocol::FAIL)
		{
			return;
		}

		const MagicTable::MagicElement magicElement = *Game::Instance().magicTable->Get(magicId);
		magic = &EntityFactory::CreateMagic(*world_, *source, magicElement);
		std::cout << "Magic Entity Added2" << std::endl;
		MagicComponent* magicComponent = magicMapper.get(*magic);
		magicComponent->GetMagicTable().startTime = 0;
		magicComponent->GetMagicTable().continueTime = continueTime;
		magicComponent->SetReady(true);
		magicComponent->SetDirection(static_cast<unsigned char>(direction));

		Entity* target = zone_->GetServerEntity(targetServerId);
		if (target != nullptr)
		{
			AttributeComponent* targetAttributeComponent = attributeMapper.get(*target);
			if (targetAttributeComponent != nullptr)
			{
				targetAttributeComponent->SetHealth(targetHealth);
				targetAttributeComponent->SetMana(targetMana);
			}
			magicComponent->GetTargets().push_back(target);
		}
	}
}

void PlayState::HandlePartyInvite(Network::GamePacket& packetRecv)
{
	sf::Uint32 leaderServerId;
	packetRecv >> leaderServerId;

	sf::Uint32 memberServerId;
	packetRecv >> memberServerId;

	std::string leaderName;
	packetRecv >> leaderName;

	Entity* leader = zone_->GetServerEntity(leaderServerId);
	if (leader != nullptr && leader == Game::Instance().myEntity)
	{
		Game::Instance().messageBoxWindow->Show("Waiting for a reply.",
			GUI::MessageBoxMode::Cancel, CEGUI::Event::Subscriber(&PlayState::HandlePartyInviteWaitingMessageBoxResult, this));
	}
	else
	{
		Game::Instance().messageBoxWindow->Show(leaderName + " has requested you to join a party.  Will you join it?",
			GUI::MessageBoxMode::OKCancel, CEGUI::Event::Subscriber(&PlayState::HandlePartyInviteRequestMessageBoxResult, this));
	}
}

bool PlayState::HandlePartyInviteWaitingMessageBoxResult(const CEGUI::EventArgs& e)
{
	SendPartyInvite(false);
	return true;
}

bool PlayState::HandlePartyInviteRequestMessageBoxResult(const CEGUI::EventArgs& e)
{
	GUI::MessageBoxResult::Enum result = Game::Instance().messageBoxWindow->GetResult();
	if (result == GUI::MessageBoxResult::OK)
	{
		SendPartyInvite(true);
	}
	else
	{
		SendPartyInvite(false);
	}
	return true;
}

void PlayState::SendPartyInvite(bool accepted)
{
	Network::GamePacket packet(Network::GameProtocol::PKT_PARTY_INVITE_RESULT);
	packet << accepted;
	Game::Instance().gameSocket->Send(packet);
}

void PlayState::HandlePartyInviteResult(Network::GamePacket& packetRecv)
{
	sf::Uint8 type;
	packetRecv >> type;

	if (type == Network::GameProtocol::INVITE_FAIL)
	{
		Game::Instance().messageBoxWindow->Show("Party request aborted.");
	}
	else if (type == Network::GameProtocol::INVITE_SUCCESS)
	{
		std::string leaderName;
		packetRecv >> leaderName;

		AttributeComponent* attributeComponent = attributeMapper.get(*Game::Instance().myEntity);
		if (attributeComponent != nullptr && attributeComponent->GetName().compare(leaderName) == 0)
		{
			Game::Instance().messageBoxWindow->Show("The party has been formed.");
		}
	}
	else if (type == Network::GameProtocol::DELETE_MEMBER)
	{
		sf::Uint32 serverId;
		packetRecv >> serverId;
		AttributeComponent* attributeComponent = attributeMapper.get(*zone_->GetServerEntity(serverId));
		if (attributeComponent != nullptr)
		{
			attributeComponent->SetPartyLeaderName("");
		}
	}
}

void PlayState::SendPartyDeny()
{
	Network::GamePacket packet(Network::GameProtocol::PKT_PARTY_DENY);
	Game::Instance().gameSocket->Send(packet);
}

void PlayState::HandleSetGray(Network::GamePacket& packetRecv)
{
	sf::Uint32 serverId;
	packetRecv >> serverId;

	sf::Uint8 grayMode;
	packetRecv >> grayMode;

	artemis::Entity* character = zone_->GetServerEntity(serverId);
	if (character != nullptr)
	{
		AttributeComponent* attributeComponent = attributeMapper.get(*character);
		if (attributeComponent != nullptr)
		{
			if (attributeComponent->GetGrayMode() == 0)
			{
				attributeComponent->RestartGrayTimer();
			}
			attributeComponent->SetGrayMode(grayMode);
		}
	}
}

void PlayState::HandleEffect(Network::GamePacket& packetRecv)
{
	sf::Uint8 mode;
	packetRecv >> mode;

	sf::Uint32 serverId;
	packetRecv >> serverId;

	if (mode == 1) // Potion Effect
	{
		sf::Uint8 r, g, b;
		packetRecv >> r >> g >> b;

		artemis::Entity* character = zone_->GetServerEntity(serverId);
		if (character != nullptr)
		{
			AttributeComponent* attributeComponent = attributeMapper.get(*character);
			if (attributeComponent != nullptr)
			{
				attributeComponent->SetFlashColor(sf::Color(r, g, b, 127));
				attributeComponent->RestartFlashColorTimer();
			}
		}
	}
}

void PlayState::HandleRestartGame(Network::GamePacket& packetRecv)
{
	engine_.ChangeState(boost::make_shared<SelectCharacterState>(engine_));
}