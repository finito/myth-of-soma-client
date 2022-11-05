
#include "Components/AttributeComponent.h"

using namespace artemis;

AttributeComponent::AttributeComponent() :
serverId_(-1), direction_(Direction::Down), skin_(0), money_(0), age_(0), moral_(MoralType::Neutral),
moralValue_(0), gender_(Gender::Male), pos_(0), health_(0), mana_(0), maximumHealth_(0), maximumMana_(0), str_(0),
dex_(0), int_(0), wis_(0), cha_(0), con_(0), level_(0), exp_(0), maximumExp_(0), weight_(0), maximumWeight_(0), stamina_(0),
maximumStamina_(0), swordExp_(0), spearExp_(0), axeExp_(0), knuckleExp_(0), bowExp_(0), staffExp_(0), weaponMakeExp_(0),
armorMakeExp_(0), accessoryMakeExp_(0), potionMakeExp_(0), cookingExp_(0), blackMagicExp_(0), whiteMagicExp_(0),
blueMagicExp_(0), state_(StateType::Idle), battleMode_(BattleMode::Normal), isAlive_(false), isAttackable_(false),
runToggle_(false), bloodType_(0), grayMode_(0), guildId_(-1), guildPictureId_(0), classType_(0)
{
}

int AttributeComponent::GetServerId() const
{
	return serverId_;
}

void AttributeComponent::SetServerId(int value)
{
	serverId_ = value;
}

Direction::Enum AttributeComponent::GetDirection() const
{
	return direction_;
}

void AttributeComponent::SetDirection(Direction::Enum value)
{
	direction_ = value;
}

void AttributeComponent::SetDirection(unsigned char value)
{
	// FIX !Assumes that the cast will always be valid!
	direction_ = static_cast<Direction::Enum>(value);
}

std::string AttributeComponent::GetGuildName() const
{
	return guildName_;
}

void AttributeComponent::SetGuildName(const std::string& value)
{
	guildName_ = value;
}

short AttributeComponent::GetGuildId() const
{
	return guildId_;
}

void AttributeComponent::SetGuildId(const short value)
{
	guildId_ = value;
}

unsigned short AttributeComponent::GetGuildPictureId() const
{
	return guildPictureId_;
}

void AttributeComponent::SetGuildPictureId(const unsigned short value)
{
	guildPictureId_ = value;
}

std::string AttributeComponent::GetName() const
{
	return name_;
}

void AttributeComponent::SetName(const std::string& value)
{
	name_ = value;
}

unsigned int AttributeComponent::GetSkin() const
{
	return skin_;
}

void AttributeComponent::SetSkin(unsigned int value)
{
	skin_ = value;
}

unsigned int AttributeComponent::GetMoney() const
{
	return money_;
}

void AttributeComponent::SetMoney(unsigned int value)
{
	money_ = value;
}

unsigned int AttributeComponent::GetAge() const
{
	return age_;
}

void AttributeComponent::SetAge(unsigned int value)
{
	age_ = value;
}

MoralType::Enum AttributeComponent::GetMoral() const
{
	return moral_;
}

void AttributeComponent::SetMoral(MoralType::Enum value)
{
	moral_ = value;
}

void AttributeComponent::SetMoral(int value)
{
	moralValue_ = value;
	if (moralValue_ <= -50)
		moral_ = MoralType::Devil;
	else if(moralValue_ >= -49 && moralValue_ <= -31)
		moral_ = MoralType::Wicked;
	else if(moralValue_ >= -30 && moralValue_ <= -11)
		moral_ = MoralType::Evil;
	else if(moralValue_ >= -10 && moralValue_ <= -4)
		moral_ = MoralType::Bad;
	else if(moralValue_ >= -3  && moralValue_ <=  3)
		moral_ = MoralType::Neutral;
	else if(moralValue_ >=  4  && moralValue_ <=  10)
		moral_ = MoralType::Good;
	else if(moralValue_ >=  11 && moralValue_ <=  30)
		moral_ = MoralType::Moral;
	else if(moralValue_ >=  31 && moralValue_ <=  49)
		moral_ = MoralType::Virtuous;
	else if(moralValue_ >=  50) 
		moral_ = MoralType::Angel;
}

int AttributeComponent::GetMoralValue() const
{
	return moralValue_;
}

Gender::Enum AttributeComponent::GetGender() const
{
	return gender_;
}

void AttributeComponent::SetGender(Gender::Enum value)
{
	gender_ = value;
}

int AttributeComponent::GetPos() const
{
	return pos_;
}

void AttributeComponent::SetPos(int value)
{
	pos_ = value;
}

unsigned short AttributeComponent::GetHealth() const
{
	return health_;
}

void AttributeComponent::SetHealth(unsigned short value)
{
	health_ = value;
}

unsigned short AttributeComponent::GetMana() const
{
	return mana_;
}

void AttributeComponent::SetMana(unsigned short value)
{
	mana_ = value;
}

unsigned short AttributeComponent::GetMaximumHealth() const
{
	return maximumHealth_;
}

void AttributeComponent::SetMaximumHealth(unsigned short value)
{
	maximumHealth_ = value;
}

unsigned short AttributeComponent::GetMaximumMana() const
{
	return maximumMana_;
}

void AttributeComponent::SetMaximumMana(unsigned short value)
{
	maximumMana_ = value;
}

unsigned short AttributeComponent::GetStr() const
{
	return str_;
}

void AttributeComponent::SetStr(unsigned short value)
{
	str_ = value;
}

unsigned short AttributeComponent::GetDex() const
{
	return dex_;
}

void AttributeComponent::SetDex(unsigned short value)
{
	dex_ = value;
}

unsigned short AttributeComponent::GetInt() const
{
	return int_;
}

void AttributeComponent::SetInt(unsigned short value)
{
	int_ = value;
}

unsigned short AttributeComponent::GetWis() const
{
	return wis_;
}

void AttributeComponent::SetWis(unsigned short value)
{
	wis_ = value;
}

unsigned short AttributeComponent::GetCha() const
{
	return cha_;
}

void AttributeComponent::SetCha(unsigned short value)
{
	cha_ = value;
}

unsigned short AttributeComponent::GetCon() const
{
	return con_;
}

void AttributeComponent::SetCon(unsigned short value)
{
	con_ = value;
}

unsigned short AttributeComponent::GetLevel() const
{
	return level_;
}

void AttributeComponent::SetLevel(unsigned short value)
{
	level_ = value;
}

unsigned int AttributeComponent::GetExp() const
{
	return exp_;
}

void AttributeComponent::SetExp(unsigned int value)
{
	exp_ = value;
}

unsigned int AttributeComponent::GetMaximumExp() const
{
	return maximumExp_;
}

void AttributeComponent::SetMaximumExp(unsigned int value)
{
	maximumExp_ = value;
}

unsigned short AttributeComponent::GetWeight() const
{
	return weight_;
}

void AttributeComponent::SetWeight(unsigned short value)
{
	weight_ = value;
}

unsigned short AttributeComponent::GetMaximumWeight() const
{
	return maximumWeight_;
}

void AttributeComponent::SetMaximumWeight(unsigned short value)
{
	maximumWeight_ = value;
}

unsigned short AttributeComponent::GetStamina() const
{
	return stamina_;
}

void AttributeComponent::SetStamina(unsigned short value)
{
	stamina_ = value;
}

void AttributeComponent::IncreaseStamina(unsigned short value)
{
	value < GetMaximumStamina() - GetStamina()
		? SetStamina(GetStamina() + value)
		: SetStamina(GetMaximumStamina());
}

unsigned short AttributeComponent::GetMaximumStamina() const
{
	return maximumStamina_;
}

void AttributeComponent::SetMaximumStamina(unsigned short value)
{
	maximumStamina_ = value;
}

unsigned int AttributeComponent::GetSwordExp() const
{
	return swordExp_;
}

void AttributeComponent::SetSwordExp(unsigned int value)
{
	swordExp_ = value;
}

unsigned int AttributeComponent::GetSpearExp() const
{
	return spearExp_;
}

void AttributeComponent::SetSpearExp(unsigned int value)
{
	spearExp_ = value;
}

unsigned int AttributeComponent::GetAxeExp() const
{
	return axeExp_;
}

void AttributeComponent::SetAxeExp(unsigned int value)
{
	axeExp_ = value;
}

unsigned int AttributeComponent::GetKnuckleExp() const
{
	return knuckleExp_;
}

void AttributeComponent::SetKnuckleExp(unsigned int value)
{
	knuckleExp_ = value;
}

unsigned int AttributeComponent::GetBowExp() const
{
	return bowExp_;
}

void AttributeComponent::SetBowExp(unsigned int value)
{
	bowExp_ = value;
}

unsigned int AttributeComponent::GetStaffExp() const
{
	return staffExp_;
}

void AttributeComponent::SetStaffExp(unsigned int value)
{
	staffExp_ = value;
}

unsigned int AttributeComponent::GetWeaponMakeExp() const
{
	return weaponMakeExp_;
}

void AttributeComponent::SetWeaponMakeExp(unsigned int value)
{
	weaponMakeExp_ = value;
}

unsigned int AttributeComponent::GetArmorMakeExp() const
{
	return armorMakeExp_;
}

void AttributeComponent::SetArmorMakeExp(unsigned int value)
{
	armorMakeExp_ = value;
}

unsigned int AttributeComponent::GetAccessoryMakeExp() const
{
	return accessoryMakeExp_;
}

void AttributeComponent::SetAccessoryMakeExp(unsigned int value)
{
	accessoryMakeExp_ = value;
}

unsigned int AttributeComponent::GetPotionMakeExp() const
{
	return potionMakeExp_;
}

void AttributeComponent::SetPotionMakeExp(unsigned int value)
{
	potionMakeExp_ = value;
}

unsigned int AttributeComponent::GetCookingExp() const
{
	return cookingExp_;
}

void AttributeComponent::SetCookingExp(unsigned int value)
{
	cookingExp_ = value;
}

unsigned int AttributeComponent::GetBlackMagicExp() const
{
	return blackMagicExp_;
}

void AttributeComponent::SetBlackMagicExp(unsigned int value)
{
	blackMagicExp_ = value;
}

unsigned int AttributeComponent::GetWhiteMagicExp() const
{
	return whiteMagicExp_;
}

void AttributeComponent::SetWhiteMagicExp(unsigned int value)
{
	whiteMagicExp_ = value;
}

unsigned int AttributeComponent::GetBlueMagicExp() const
{
	return blueMagicExp_;
}

void AttributeComponent::SetBlueMagicExp(unsigned int value)
{
	blueMagicExp_ = value;
}

StateType::Enum AttributeComponent::GetState() const
{
	return state_;
}

void AttributeComponent::SetState(StateType::Enum value)
{
	state_ = value;
}

BattleMode::Enum AttributeComponent::GetBattleMode() const
{
	return battleMode_;
}

void AttributeComponent::SetBattleMode(BattleMode::Enum  value)
{
	battleMode_ = value;
}

void AttributeComponent::SetBattleMode(unsigned char value)
{
	// FIX !Assumes that the cast will always be valid!
	battleMode_ = static_cast<BattleMode::Enum>(value);
}

bool AttributeComponent::IsAlive() const
{
	return isAlive_;
}

void AttributeComponent::SetAlive(bool value)
{
	isAlive_ = value;
}

bool AttributeComponent::IsAttackable() const
{
	return isAttackable_;
}

void AttributeComponent::SetAttackable(bool value)
{
	isAttackable_ = value;
}

float AttributeComponent::GetHealthStatus() const
{
	return static_cast<float>(health_) / maximumHealth_;
}

float AttributeComponent::GetManaStatus() const
{
	return static_cast<float>(mana_) / maximumMana_;
}

float AttributeComponent::GetStaminaStatus() const
{
	return static_cast<float>(stamina_) / maximumStamina_;
}

float AttributeComponent::GetWeightStatus() const
{
	return static_cast<float>(weight_) / maximumWeight_;
}

float AttributeComponent::GetExpStatus() const
{
	return static_cast<float>(exp_) / maximumExp_;
}

unsigned short AttributeComponent::GetHair() const
{
	return hair_;
}

void AttributeComponent::SetHair(unsigned short value)
{
	hair_ = value;
}

void AttributeComponent::SetRunToggle(bool value)
{
	runToggle_ = value;
}

bool AttributeComponent::GetRunToggle() const
{
	return runToggle_;
}

void AttributeComponent::SetBloodType(int value)
{
	bloodType_ = value;
}

int AttributeComponent::GetBloodType() const
{
	return bloodType_;
}

void AttributeComponent::SetPartyLeaderName(const std::string& value)
{
	partyLeaderName_ = value;
}

std::string AttributeComponent::GetPartyLeaderName() const
{
	return partyLeaderName_;
}

void AttributeComponent::SetGrayMode(const unsigned char value)
{
	grayMode_ = value;
}

unsigned char AttributeComponent::GetGrayMode() const
{
	return grayMode_;
}

bool AttributeComponent::IsGray() const
{
	return grayMode_ != 0;
}

void AttributeComponent::RestartGrayTimer()
{
	grayTimer_.restart();
}

const sf::Clock& AttributeComponent::GetGrayTimer() const
{
	return grayTimer_;
}

void AttributeComponent::SetFlashColor(sf::Color value)
{
	flashColor_ = value;
}

const sf::Color AttributeComponent::GetFlashColor() const
{
	return flashColor_;
}

void AttributeComponent::RestartFlashColorTimer()
{
	flashTimer_.restart();
}

const sf::Clock& AttributeComponent::GetFlashColorTimer() const
{
	return flashTimer_;
}

void AttributeComponent::SetClassType(unsigned short value)
{
	classType_ = value;
}

unsigned int AttributeComponent::GetClassType() const
{
	return classType_;
}

std::map<unsigned short, Entity*>& AttributeComponent::GetMagicEntityMap()
{
	return magicEntityMap_;
}

std::map<unsigned short, Entity&>& AttributeComponent::GetContinueEffectEntities()
{
	return magicEffectEntities_;
}
