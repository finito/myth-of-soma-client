
#include "GUI/StatusWindow.h"

#include "Components/AttributeComponent.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;

StatusWindow::StatusWindow()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("statuswindow.layout");
		window_->hide();
		window_->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&StatusWindow::HandleWindowUpdated, this));
		window_->subscribeEvent(Window::EventShown, Event::Subscriber(&StatusWindow::HandleOpen, this));

		name_ = window_->getChild("Status/Name");
		knownAs_ = window_->getChild("Status/Knownas");
		guildName_ = window_->getChild("Status/Guild");
		pos_ = window_->getChild("Status/Pos");
		level_ = window_->getChild("Status/Level");
		age_ = window_->getChild("Status/Age");
		gender_ = window_->getChild("Status/Gender");
		moral_ = window_->getChild("Status/Moral");
		health_ = window_->getChild("Status/CurrentHealth");
		maximumHealth_ = window_->getChild("Status/MaxHealth");
		mana_ = window_->getChild("Status/CurrentMana");
		maximumMana_ = window_->getChild("Status/MaxMana");
		stamina_ = window_->getChild("Status/CurrentStamina");
		maximumStamina_ = window_->getChild("Status/MaxStamina");
		strength_ = window_->getChild("Status/Strength");
		intelligence_ = window_->getChild("Status/Intelligence");
		dexterity_ = window_->getChild("Status/Dexterity");
		wisdom_ = window_->getChild("Status/Wisdom");
		charisma_ = window_->getChild("Status/Charisma");
		constitution_ = window_->getChild("Status/Constitution");
		swordSkill_ = window_->getChild("Status/SwordSkill");
		spearSkill_ = window_->getChild("Status/SpearSkill");
		axeSkill_ = window_->getChild("Status/AxeSkill");
		knuckleSkill_ = window_->getChild("Status/KnuckleSkill");
		bowSkill_ = window_->getChild("Status/BowSkill");
		staffSkill_ = window_->getChild("Status/StaffSkill");
		armorCraftSkill_ = window_->getChild("Status/ArmorCraftSkill");
		weaponCraftSkill_ = window_->getChild("Status/WeaponCraftSkill");
		accessoryCraftSkill_ = window_->getChild("Status/AccessoryCraftSkill");
		potionCraftSkill_ = window_->getChild("Status/PotionCraftSkill");
		cookingCraftSkill_ = window_->getChild("Status/CookingCraftSkill");
		blackMagicSkill_ = window_->getChild("Status/BlackMagicSkill");
		whiteMagicSkill_ = window_->getChild("Status/WhiteMagicSkill");
		blueMagicSkill_ = window_->getChild("Status/BlueMagicSkill");

		closeButton_ = static_cast<PushButton*>(window_->getChild("Status/CloseButton"));
		closeButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&StatusWindow::HandleClose, this));

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "StatusWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

StatusWindow::~StatusWindow()
{
	window_->destroy();
}

void StatusWindow::Update()
{
	AttributeComponent* attributeComponent = Game::Instance().myEntity ? Game::Instance().myEntity->getComponent<AttributeComponent>() : nullptr;
	if (attributeComponent == nullptr)
	{
		return;
	}

	name_->setText(attributeComponent->GetName());
	guildName_->setText(attributeComponent->GetGuildName());
	pos_->setText(PropertyHelper::intToString(attributeComponent->GetPos()));
	level_->setText(PropertyHelper::intToString(attributeComponent->GetLevel()));
	age_->setText(PropertyHelper::intToString(attributeComponent->GetAge()));

	std::string gender;
	switch (attributeComponent->GetGender())
	{
	case Gender::Male:
		gender = "M";
		break;
	case Gender::Female:
		gender = "F";
		break;
	}
	gender_->setText(gender);

	moralValue_.SetValue(attributeComponent->GetMoralValue());
	moral_->setText(moralValue_.GetText());
	moral_->setProperty("TextColours", PropertyHelper::colourToString(moralValue_.GetColor()));

	health_->setText(PropertyHelper::intToString(attributeComponent->GetHealth()));
	maximumHealth_->setText(PropertyHelper::intToString(attributeComponent->GetMaximumHealth()));
	mana_->setText(PropertyHelper::intToString(attributeComponent->GetMana()));
	maximumMana_->setText(PropertyHelper::intToString(attributeComponent->GetMaximumMana()));
	stamina_->setText(PropertyHelper::intToString(attributeComponent->GetStamina()));
	maximumStamina_->setText(PropertyHelper::intToString(attributeComponent->GetMaximumStamina()));

	strength_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetStr() / 10.f)));
	intelligence_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetInt() / 10.f)));
	dexterity_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetDex() / 10.f)));
	wisdom_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetWis() / 10.f)));
	charisma_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetCha() / 10.f)));
	constitution_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetCon() / 10.f)));

	swordSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetSwordExp() / 10.f)));
	spearSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetSpearExp() / 10.f)));
	axeSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetAxeExp() / 10.f)));
	knuckleSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetKnuckleExp() / 10.f)));
	bowSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetBowExp() / 10.f)));
	staffSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetStaffExp() / 10.f)));

	armorCraftSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetArmorMakeExp() / 10.f)));
	weaponCraftSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetWeaponMakeExp() / 10.f)));
	accessoryCraftSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetAccessoryMakeExp() / 10.f)));
	potionCraftSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetPotionMakeExp() / 10.f)));
	cookingCraftSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetCookingExp() / 10.f)));

	blackMagicSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetBlackMagicExp() / 10.f)));
	whiteMagicSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetWhiteMagicExp() / 10.f)));
	blueMagicSkill_->setText(boost::str(boost::format("%1$.1f") % (attributeComponent->GetBlueMagicExp() / 10.f)));
}

bool StatusWindow::HandleOpen(const EventArgs& e)
{
	Update();
	return true;
}

bool StatusWindow::HandleClose(const EventArgs& e)
{
	window_->hide();
	return true;
}

bool StatusWindow::HandleWindowUpdated(const EventArgs& e)
{
	Update();
	return true;
}

} // namespace GUI