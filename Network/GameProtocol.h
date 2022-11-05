#ifndef GAME_PROTOCOL_H
#define GAME_PROTOCOL_H

namespace Network
{

namespace GameProtocol
{

// Packet identifiers
const unsigned char PKT_ACCOUNT_LOGIN			= 0xfe; // Char select login
const unsigned char PKT_NEW_HERO				= 0x40; // Character creation
const unsigned char PKT_DEL_HERO				= 0x41; // Character deletion
const unsigned char PKT_LOGIN					= 0x01; // Game login
const unsigned char PKT_LOGOUT					= 0x02; // Game Logout
const unsigned char PKT_USERMODIFY				= 0X09; // User add / delete
const unsigned char PKT_CHARACTER_DATA			= 0x50;	// User stat data
const unsigned char PKT_CHAT					= 0x07;
const unsigned char PKT_SET_TIME				= 0x29;
const unsigned char PKT_GAMESTART				= 0xE9;
const unsigned char PKT_MOVEFIRST				= 0x03;
const unsigned char PKT_MOVEMIDDLE				= 0x04;
const unsigned char PKT_MOVEEND					= 0x06;
const unsigned char PKT_RUN_MOVEFIRST			= 0x15;
const unsigned char PKT_RUN_MOVEMIDDLE			= 0x16;
const unsigned char PKT_RUN_MOVEEND				= 0x17;
const unsigned char PKT_RESTARTGAME				= 0x0c; // Restart Game (Goes to char select)
const unsigned char PKT_INV_ALL					= 0x30;
const unsigned char PKT_CHANGE_ITEM_INDEX		= 0x32;
const unsigned char PKT_ITEM_INFO				= 0x33;
const unsigned char PKT_ITEM_CHANGE_INFO		= 0x3d;
const unsigned char PKT_BATTLEMODE				= 0x18;
const unsigned char PKT_CHANGEBELTINDEX			= 0x89;
const unsigned char PKT_PUTITEM_INV				= 0x88;
const unsigned char PKT_PUTITEM_BELT			= 0x87;
const unsigned char PKT_ITEM_THROW				= 0x34;
const unsigned char PKT_WEIGHT					= 0x3a;
const unsigned char PKT_ITEM_FIELD_INFO			= 0x36;
const unsigned char PKT_ITEM_DUMP				= 0x3e;
const unsigned char PKT_MONEYCHANGE				= 0x39;
const unsigned char PKT_ITEM_PICKUP				= 0x35;
const unsigned char PKT_MAGIC_ALL				= 0x26;
const unsigned char PKT_MAGIC_READY				= 0x13; 
const unsigned char PKT_ITEM_USE				= 0x3c;
const unsigned char PKT_EFFECT					= 0xaa;
const unsigned char PKT_SETRUNMODE				= 0xda;
const unsigned char PKT_CHANGEDIR				= 0xa7;
const unsigned char PKT_LIFE					= 0x25;
const unsigned char PKT_HPMP_RECOVERY			= 0x96; 
const unsigned char PKT_CLIENTEVENT				= 0x60;
const unsigned char PKT_EVENTSELBOX				= 0x92;
const unsigned char PKT_CLASSPOINTBUY			= 0xe5;
const unsigned char PKT_GETITEM					= 0x37;
const unsigned char PKT_ZONECHANGE				= 0x0a;
const unsigned char PKT_EVENTOKBOX				= 0x93;
const unsigned char PKT_EVENTNORMAL				= 0x94;
const unsigned char PKT_REPAIR_OPEN				= 0x99;
const unsigned char PKT_SKILL_ABILITY			= 0xd4; // Crafting skills
const unsigned char PKT_MAKEROPEN				= 0x74;	
const unsigned char PKT_OUTITEM					= 0x38;
const unsigned char PKT_SHOW_MAGIC				= 0x2a;
const unsigned char PKT_STORAGEOPEN				= 0x76;
const unsigned char PKT_SHOPOPEN				= 0x70;
const unsigned char PKT_TOWNPORTALREQ			= 0xa8;
const unsigned char PKT_TOWNPORTALEND			= 0xa9;
const unsigned char PKT_SHOP_ITEM_COUNT			= 0x71;
const unsigned char PKT_SELL_SHOP				= 0x72;
const unsigned char PKT_BUY_SHOP				= 0x73;
const unsigned char PKT_SAVEITEM				= 0x77;
const unsigned char PKT_TAKEBACKITEM			= 0x78;
const unsigned char PKT_SAVEMONEY				= 0x79;
const unsigned char PKT_TAKEBACKMONEY			= 0x7a;
const unsigned char PKT_ATTACK					= 0x10;
const unsigned char PKT_ATTACK_MAGIC_ARROW		= 0x11; // Magic Attack not bow
const unsigned char PKT_ATTACK_MAGIC_RAIL		= 0X12;
const unsigned char PKT_ATTACK_MAGIC_CIRCLE    	= 0x14;  
const unsigned char PKT_STATUS_INFO				= 0X19;  
const unsigned char PKT_POISON					= 0x20; 
const unsigned char PKT_ENCRYPTION_START_REQ	= 0xfd;
const unsigned char ENCRYPTION_PKT				= 0xf9;
const unsigned char PKT_DUMMY_ATTACK			= 0xc7;
const unsigned char PKT_DEAD					= 0x23;
const unsigned char PKT_SPECIALATTACKCANCEL		= 0xc3;
const unsigned char PKT_PARTY_DENY				= 0xcc;
const unsigned char PKT_PARTY_INVITE			= 0xcd;
const unsigned char PKT_PARTY_INVITE_RESULT		= 0xce;
const unsigned char PKT_SETGRAY					= 0xad; // gray name for pker etc
const unsigned char PKT_RESURRECTION_SCROLL		= 0xd1;
const unsigned char PKT_MAGIC_BELT				= 0x27;
const unsigned char PKT_CHANGE_OTHER_ITEM		= 0xe1;
const unsigned char PKT_BBS_OPEN				= 0x65;
const unsigned char PKT_BBS_NEXT				= 0x66;
const unsigned char PKT_BBS_READ				= 0x67;
const unsigned char PKT_BBS_WRITE				= 0x68;
const unsigned char PKT_BBS_EDIT				= 0x69;
const unsigned char PKT_BBS_DELETE				= 0x6A;
const unsigned char PKT_TRADEREXCHANGE			= 0xDB;
const unsigned char PKT_GUILD_OPEN				= 0x97;
const unsigned char PKT_GUILD_LIST				= 0x98;
const unsigned char PKT_GUILD_MEMBER_LIST		= 0xa2;
const unsigned char PKT_GUILD_NEW				= 0x95;
const unsigned char PKT_GUILD_INFO				= 0x9f;
const unsigned char PKT_GUILD_CHANGE_POS		= 0xa6;
const unsigned char PKT_GUILD_EDIT_INFO_REQ		= 0xae;
const unsigned char PKT_GUILD_EDIT_INFO			= 0xaf;
const unsigned char PKT_GUILD_APPLICATION_LIST	= 0xa4;
const unsigned char PKT_GUILD_REQ_USER_INFO		= 0xa1;
const unsigned char PKT_GUILD_REQ				= 0x9a;
const unsigned char PKT_GUILD_MOVE_REQ			= 0x9b;
const unsigned char PKT_GUILD_REMOVE_REQ		= 0x9c;
const unsigned char PKT_GUILD_MOVE_REJECT		= 0xa3;
const unsigned char PKT_GUILD_CHANGE_RANK		= 0xa5;
const unsigned char PKT_GUILD_MONEY		        = 0xbd;
const unsigned char PKT_GUILD_REMOVE_USER		= 0x9e;
const unsigned char PKT_GUILD_DISBAND			= 0x9d;
const unsigned char PKT_REPAIR_REQ				= 0xa0;
const unsigned char PKT_MESSAGE					= 0xc8;
const unsigned char PKT_ALLCHAT_STATUS			= 0xcb;
const unsigned char PKT_MAGICPRECAST			= 0xdc;
const unsigned char PKT_QUEST_VIEW				= 0xdf;
const unsigned char PKT_ATTACK_SPECIAL_ARROW	= 0xbe;
const unsigned char PKT_ATTACK_SPECIAL_RAIL		= 0xbf;
const unsigned char PKT_ATTACK_SPECIAL_CIRCLE	= 0xc0;
const unsigned char PKT_ATTACK_SPECIAL_CIRCLE2	= 0xc1;
const unsigned char PKT_HAIRSHOPOK				= 0xd2;
const unsigned char PKT_GUILDWAR_REQ			= 0xb2;
const unsigned char PKT_GUILDWAR_REQ_RESULT		= 0xb3;
const unsigned char PKT_GUILDWAR_REQ_CANCEL		= 0xb4;
const unsigned char PKT_GUILDWAR_OK				= 0xb5;
const unsigned char PKT_GUILD_STORAGEOPEN		= 0xb6;
const unsigned char PKT_GUILD_SAVEITEM			= 0xb7;
const unsigned char PKT_GUILD_TAKEBACKITEM		= 0xb8;
const unsigned char PKT_GUILD_STORAGECLOSE		= 0xb9;
const unsigned char PKT_GUILD_SYMBOL_CHANGE_REQ = 0xb0;
const unsigned char PKT_GUILD_SYMBOL_CHANGE		= 0xb1;
const unsigned char PKT_GUILD_SYMBOL_DATA		= 0xd3;
const unsigned char PKT_TRADE_REQ				= 0x7b;
const unsigned char PKT_TRADE_ACK				= 0x7c;
const unsigned char PKT_TRADE_ADDITEM			= 0x7d;
const unsigned char PKT_TRADE_SETMONEY			= 0x7e;
const unsigned char PKT_TRADE_OK				= 0x7f;
const unsigned char PKT_TRADE_CANCEL			= 0x80;
const unsigned char PKT_TRADE_RESULT			= 0x81;
const unsigned char PKT_GUILDTOWNSTONE_OPEN		= 0xd9;
const unsigned char PKT_SPECIAL_MOVE			= 0xd6;
const unsigned char PKT_GUILD_STORAGE_LIMIT_CHANGE = 0xd0;
const unsigned char PKT_MAKEITEM                = 0x75;

// Length constants
const int ACCOUNT_LENGTH  = 12; // Length of the login id and password
const int NAME_LENGTH	  = 12; // Length of character names
const int NPC_NAME_LENGTH = 50; // Length of NPC names

// Error codes
const unsigned char SUCCESS		= 1;
const unsigned char FAIL		= 2;
const unsigned char ERR_1		= 1;
const unsigned char ERR_2		= 2;
const unsigned char ERR_3		= 3;
const unsigned char ERR_4		= 4;
const unsigned char ERR_5		= 5;
const unsigned char ERR_6		= 6;
const unsigned char ERR_7		= 7;
const unsigned char ERR_8		= 8;
const unsigned char ERR_9		= 9;
const unsigned char ERR_10		= 10;
const unsigned char ERR_11		= 11;
const unsigned char ERR_12		= 12;
const unsigned char UNKNOWN_ERR	= 255;

// Item constants
const int EQUIP_ITEM_NUM		 = 10;
const int INV_ITEM_NUM			 = 50;
const int BELT_ITEM_NUM			 = 4;
const int STORAGE_ITEM_NUM		 = 80;
const int GUILD_STORAGE_ITEM_NUM = 40;

// Char constants
const int GENDER_FEMALE	= 0;
const int GENDER_MALE	= 1;

// UserModify packet constants
const unsigned char INFO_MODIFY	= 1;
const unsigned char INFO_DELETE	= 2;

// Character Data packet constants
const unsigned char INFO_NAMES		= 0x01;
const unsigned char INFO_BASICVALUE = 0x02;			
const unsigned char INFO_EXTVALUE   = 0x04;			
const unsigned char INFO_WEAPONEXP  = 0x08;			
const unsigned char INFO_MAKEEXP    = 0x10;			
const unsigned char INFO_MAGICEXP   = 0x20;			
const unsigned char INFO_ALL	    = 0xFF;

// Item Data packet constants
const unsigned char GET_VALUE		= 0x01;
const unsigned char GET_EXTRA_USAGE = 0x02;
const unsigned char NORM_LAYOUT		= 0x04;
const unsigned char TRADE_LAYOUT	= 0x08;
const unsigned char SHOP_LAYOUT		= 0x10;
const unsigned char AUCTION_LAYOUT  = 0x20;
const unsigned char ITEM_STORAGE	= NORM_LAYOUT;
const unsigned char ITEM_ME			= GET_VALUE | GET_EXTRA_USAGE | NORM_LAYOUT;
const unsigned char ITEM_EXCHANGE	= TRADE_LAYOUT;
const unsigned char ITEM_SHOP		= SHOP_LAYOUT | NORM_LAYOUT;
const unsigned char ITEM_AUCTION	= AUCTION_LAYOUT | NORM_LAYOUT;

// Item Change Data packet constants
const unsigned char INFO_TYPE	 = 0x01;
const unsigned char INFO_ARM	 = 0x02;
const unsigned char INFO_PICNUM	 = 0x04;
const unsigned char INFO_DUR	 = 0x08;
const unsigned char INFO_NAME	 = 0x10;
const unsigned char INFO_DAMAGE  = 0x20;
const unsigned char INFO_LIMIT	 = 0x40;
const unsigned char INFO_SPECIAL = 0x80;


// Chat type packet constants
namespace ChatType
{
	const unsigned char NORMAL			 = 1;
	const unsigned char WHISPER			 = 2;
	const unsigned char ZONE			 = 3;
	const unsigned char WHISPER_ME		 = 4;
	const unsigned char SYSTEM			 = 5;
	const unsigned char GM_NOTICE		 = 6;
	const unsigned char GUILD			 = 7;
	const unsigned char SHOUT			 = 8;
	const unsigned char YELLOW_STAT		 = 9;
	const unsigned char BLUE_STAT		 = 10;
	const unsigned char PARTY			 = 11;
	const unsigned char GM_SCROLL_NOTICE = 12;
	const unsigned char LAST			 = 13;
}

// Time Set packet constants
const unsigned char  WEATHER_FINE = 0x00;
const unsigned char  WEATHER_RAIN = 0x01;
//const unsigned char  WEATHER_SNOW		= 0x03;

// Field item info packet constants
const unsigned char ITEM_INFO_MODIFY = 1;
const unsigned char ITEM_INFO_DELETE = 2;

// Magic packet constants
const unsigned char MAGIC_TYPE_MAGIC   = 1;
const unsigned char MAGIC_TYPE_SPECIAL = 0;
const unsigned char MAGIC_TYPE_ABILITY = 4;

// Party Invite packet constants
const unsigned char INVITE_FAIL    = 0;
const unsigned char INVITE_SUCCESS = 1;
const unsigned char DELETE_MEMBER  = 2;

// Repair failure packet constants
const unsigned char FAIL_REPAIR	  = 0;
const unsigned char FAIL_NONEED   = 1;
const unsigned char FAIL_NOREPAIR = 2;
const unsigned char FAIL_NOMONEY  = 3;
const unsigned char FAIL_DESTROY  = 4;

// PKT_ATTACK results
const unsigned char ATTACK_SUCCESS = 1;
const unsigned char	ATTACK_FAIL	   = 2;
const unsigned char	ATTACK_MISS	   = 3;

// Guild packet failure constants
const unsigned char GUILD_INVALID_GUILD_NAME = 1;
const unsigned char GUILD_INVALID_GUILD_INFO = 2;
const unsigned char GUILD_FULL = 3;
const unsigned char GUILD_SYSTEM_ERROR = 4;
const unsigned char GUILD_ALREADY_JOIN = 5;
const unsigned char GUILD_SMALL_LEVEL = 6;
const unsigned char GUILD_SAME_GUILD_NAME = 7;
const unsigned char GUILD_ABSENT_JOIN = 8;
const unsigned char GUILD_ABSENT_REQ_USER = 9;
const unsigned char GUILD_ABSENT_GUILD_NAME = 10;
const unsigned char GUILD_SMALL_RANK = 11;
const unsigned char GUILD_ABSENT_JOIN_REQ = 12;
const unsigned char GUILD_ALREADY_JOIN_REQ = 13;
const unsigned char GUILD_NEED_EMPTY_RANK = 14;
const unsigned char GUILD_NOT_GUILD_USER = 15;
const unsigned char GUILD_INVALID_RANK = 16;
const unsigned char GUILD_INVALID_GUILD_CALL_NAME = 17;
const unsigned char GUILD_SMALL_MONEY = 18;
const unsigned char GUILD_MEMBER_NOT_ONLINE = 22;

// Guild money packet constants
const unsigned char GUILD_MONEY_PUT = 1;
const unsigned char GUILD_MONEY_GET = 2;

// PKT_SKILL_ABILITY Ability Types
static const unsigned char ABILITY_OPEN = 0;
static const unsigned char ABILITY_REPAIR = 1;
static const unsigned char ABILITY_DISASSEMBLE = 2;
static const unsigned char ABILITY_SYNTHESIS = 3;
static const unsigned char ABILITY_SMELT = 4;
static const unsigned char ABILITY_PREPARE = 5;
static const unsigned char ABILITY_UPGRADE = 8;

} // namespace GameProtocol

} // namespace Network

#endif // GAME_PROTOCOL_H