
#include "GUI/ItemTooltip.h"

#include "Components/ItemAttributeComponent.h"
#include "Components/AttributeComponent.h"

#include <iostream>
#include <boost/format.hpp>

#include "Game.h"

namespace GUI
{

using namespace CEGUI;

ItemTooltip::ItemTooltip()
{
	try
	{
		window_ = WindowManager::getSingleton().loadWindowLayout("itemtooltip.layout");
		window_->hide();

		classType_		= window_->getChild("ItemTooltip/ClassType");
		name_			= window_->getChild("ItemTooltip/Name");
		stats_			= window_->getChild("ItemTooltip/Stats");
		requirements_	= window_->getChild("ItemTooltip/Requirements");
		special_		= window_->getChild("ItemTooltip/Special");
		repairEffect_	= window_->getChild("ItemTooltip/RepairEffect");
		repairInfo_		= window_->getChild("ItemTooltip/RepairInfo");

		window_->setAlwaysOnTop(true);

		System::getSingleton().getGUISheet()->addChildWindow(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "ItemTooltip: Failed to initialize: " << e.what() << std::endl;
	}
}

ItemTooltip::~ItemTooltip()
{
	window_->destroy();
}

void ItemTooltip::Update(const ItemAttributeComponent& item)
{
	classType_->hide();
	requirements_->hide();
	special_->hide();
	repairEffect_->hide();
	repairInfo_->hide();

	SetClassType(item);
	SetName(item);
	SetStats(item);
	SetRequirements(item);
	SetSpecial(item);

	window_->setWidth(UDim(0, 210.f));

	float width = 0;
	float height = 4;

	classType_->setPosition(UVector2(UDim(0, 0), UDim(0, height)));

	float classTypeWidth = PropertyHelper::stringToFloat(classType_->getProperty("HorzExtent"));
	if (classTypeWidth > width)
	{
		width = classTypeWidth;
	}

	float classTypeHeight = PropertyHelper::stringToFloat(classType_->getProperty("VertExtent"));
	height += classTypeHeight;
	classType_->setHeight(UDim(0, classTypeHeight));

	name_->setPosition(UVector2(UDim(0, 0), UDim(0, height)));

	float nameWidth = PropertyHelper::stringToFloat(name_->getProperty("HorzExtent"));
	if (nameWidth > width)
	{
		width = nameWidth;
	}

	float nameHeight = PropertyHelper::stringToFloat(name_->getProperty("VertExtent"));
	height += nameHeight;
	name_->setHeight(UDim(0, nameHeight));

	stats_->setPosition(UVector2(UDim(0, 0), UDim(0, height)));

	float statsWidth = PropertyHelper::stringToFloat(stats_->getProperty("HorzExtent"));
	if (statsWidth > width)
	{
		width = statsWidth;
	}

	float statsHeight = PropertyHelper::stringToFloat(stats_->getProperty("VertExtent"));
	height += statsHeight;
	stats_->setHeight(UDim(0, statsHeight));

	requirements_->setPosition(UVector2(UDim(0, 0), UDim(0, height)));

	float requirementsWidth = PropertyHelper::stringToFloat(requirements_->getProperty("HorzExtent"));
	if (requirementsWidth > width)
	{
		width = requirementsWidth;
	}

	float requirementsHeight = PropertyHelper::stringToFloat(requirements_->getProperty("VertExtent"));
	height += requirementsHeight;
	requirements_->setHeight(UDim(0, requirementsHeight));

	special_->setPosition(UVector2(UDim(0, 0), UDim(0, height)));

	float specialWidth = PropertyHelper::stringToFloat(special_->getProperty("HorzExtent"));
	if (specialWidth > width)
	{
		width = specialWidth;
	}

	float specialHeight = PropertyHelper::stringToFloat(special_->getProperty("VertExtent"));
	height += specialHeight;
	special_->setHeight(UDim(0, specialHeight));

	window_->setWidth(UDim(0, width+8));
	window_->setHeight(UDim(0, height+4));
}

void ItemTooltip::Show(const UVector2& position)
{
	window_->setPosition(position);
	window_->show();
}

void ItemTooltip::Hide()
{
	window_->hide();
}

void ItemTooltip::SetClassType(const ItemAttributeComponent& item)
{
	std::string classType;

	AttributeComponent* attributeComponent = Game::Instance().myEntity->getComponent<AttributeComponent>();
	if (attributeComponent != nullptr)
	{
		if (item.GetClassType() == ItemClassType::Human && attributeComponent->GetClassType() >= 10)
		{
			classType = "!Human's World Item";
		}
		else if (item.GetClassType() == ItemClassType::Devil && attributeComponent->GetClassType() < 10)
		{
			classType = "!Devil's World Item";
		}
	}

	classType_->setText(classType);
	if (!classType.empty())
	{
		classType_->show();
	}
}

void ItemTooltip::SetName(const ItemAttributeComponent& item)
{
	std::string gender;
	switch (item.GetGender())
	{
	case ItemGender::Female:
		gender = "(F)";
		break;
	case ItemGender::Male:
		gender = "(M)";
		break;
	default:
		gender = "";
		break;
	}

	std::string name;
	if (item.GetShopQuantity() <= 0)
	{
		switch (item.GetType())
		{
		case ItemType::Potion:
		case ItemType::Food:
		case ItemType::Material:
		case ItemType::Token:
		case ItemType::Quest:
			if (item.GetShopQuantity() == -1)
			{
				name = boost::str(boost::format("%1%") % item.GetName());
			}
			else
			{
				name = boost::str(boost::format("%1% (%2%)") % item.GetName() % item.GetDura());
			}
			break;
		default:
			name = boost::str(boost::format("%1% %2%") % item.GetName() % gender);
			break;
		}
	}
	else
	{
		name = boost::str(boost::format("%1% (%2%)") % item.GetName() % item.GetShopQuantity());
	}	

	if (item.GetUpgradeLevel() > 0)
	{
		name.append(boost::str(boost::format("+ %1%") % item.GetUpgradeLevel()));
	}

	name_->setText(name);
}

void ItemTooltip::SetStats(const ItemAttributeComponent& item)
{
	std::string stats;
	switch (item.GetType())
	{
	case ItemType::Sword:
	case ItemType::Axe:
	case ItemType::Bow:
	case ItemType::Spear:
	case ItemType::Knuckle:
	case ItemType::Staff:
		stats = boost::str(boost::format("Attack : %1%~%2%\nWeight : %3%\nDurability : %4%\nSpeed : %5%")
			% item.GetMinimumDamage() % item.GetMaximumDamage() % item.GetWeight() % item.GetDura() % item.GetSpeed());
		break;
	case ItemType::Armor:
		stats = boost::str(boost::format("Defense : %1%~%2%\nWeight : %3%\nDurability : %4%")
			% item.GetMinimumDamage() % item.GetMaximumDamage() % item.GetWeight() % item.GetDura());
		break;
	case ItemType::Accessory:
		stats = boost::str(boost::format("Weight : %1%\nDurability : %2%") % item.GetWeight() % item.GetDura());
		break;
	default:
		stats = boost::str(boost::format("Weight : %1%") % item.GetWeight());
		break;
	}

	stats_->setText(stats);
}

void ItemTooltip::SetRequirements(const ItemAttributeComponent& item)
{
	std::string requirements;
	switch (item.GetType())
	{
	case ItemType::Sword:
	case ItemType::Axe:
	case ItemType::Bow:
	case ItemType::Spear:
	case ItemType::Knuckle:
	case ItemType::Staff:
	case ItemType::Armor:
	case ItemType::Accessory:
		if (item.GetRequirementStr() > 0)
		{
			requirements.append(boost::str(boost::format("Req. Str : %1%\n") % item.GetRequirementStr()));
		}
		if (item.GetRequirementInt() > 0)
		{
			requirements.append(boost::str(boost::format("Req. Int : %1%\n") % item.GetRequirementInt()));
		}
		if (item.GetRequirementDex() > 0)
		{
			requirements.append(boost::str(boost::format("Req. Dex : %1%\n") % item.GetRequirementDex()));
		}
		break;
	}

	switch (item.GetType())
	{
	case ItemType::Sword:
	case ItemType::Axe:
	case ItemType::Bow:
	case ItemType::Spear:
	case ItemType::Knuckle:
	case ItemType::Staff:
		if (item.GetRequirementSkill() > 0)
		{
			requirements.append(boost::str(boost::format("Req. Skill : %1%") % item.GetRequirementSkill()));
		}
		break;
	}

	requirements_->setText(requirements);
	if (!requirements.empty())
	{
		requirements_->show();
	}
}

void ItemTooltip::SetSpecial(const ItemAttributeComponent& item)
{
	std::string special;

	if (item.GetHealth() > 0)
	{
		special.append(boost::str(boost::format("HP Recovery+%1%\n") % item.GetHealth()));
	}

	if (item.GetMana() > 0)
	{
		special.append(boost::str(boost::format("MP Recovery+%1%\n") % item.GetMana()));
	}

	for (int i = 0; i < ItemAttributeComponent::SPECIAL_COUNT; ++i)
	{
		switch (item.GetSpecialId(i))
		{
		case ItemSpecial::MagicAttack: // Fall-through!
		case ItemSpecial::MagicDefense:
			special.append(boost::str(boost::format(ItemSpecial::ToString(item.GetSpecialId(i)))
				% item.GetSpecialOption(i, 0) % item.GetSpecialOption(i, 1)));

			if (item.GetSpecialOption(i, 2) == 0)
			{
				special.append("(100%)");
			}
			else
			{
				special.append(boost::str(boost::format("(%1%%%)") % item.GetSpecialOption(i, 2)));
			}
			break;
		case ItemSpecial::PhysicalAttack:  // Fall-through!
		case ItemSpecial::PhysicalDefense:
		case ItemSpecial::MagicAttack2:
		case ItemSpecial::MagicDefense2:
		case ItemSpecial::BlueMagicAttack:
		case ItemSpecial::WhiteMagicAttack:
		case ItemSpecial::BlackMagicAttack:
			special.append(boost::str(boost::format(ItemSpecial::ToString(item.GetSpecialId(i)))
				% item.GetSpecialOption(i, 0)));

			if (item.GetSpecialOption(i, 1) > 0)
			{
				special.append(boost::str(boost::format("~%1%") % item.GetSpecialOption(i, 1)));
			}

			if (item.GetSpecialOption(i, 2) > 0)
			{
				special.append(boost::str(boost::format("(%1%%%)") % item.GetSpecialOption(i, 2)));
			}
			break;
		case ItemSpecial::Evasion: // Fall-through!
		case ItemSpecial::Accuracy:
			special.append(boost::str(boost::format(ItemSpecial::ToString(item.GetSpecialId(i)))
				% item.GetSpecialOption(i, 0)));

			if (item.GetSpecialOption(i, 1) > 0)
			{
				special.append(boost::str(boost::format("~%1%%%") % item.GetSpecialOption(i, 1)));
			}

			if (item.GetSpecialOption(i, 2) > 0)
			{
				special.append(boost::str(boost::format("(%1%%%)") % item.GetSpecialOption(i, 2)));
			}
			break;
		case ItemSpecial::Strength: // Fall-through!
		case ItemSpecial::Intelligence:
		case ItemSpecial::Dexterity:
		case ItemSpecial::Wisdom:
		case ItemSpecial::Constitution:
		case ItemSpecial::Charisma:
		case ItemSpecial::MaximumHealth:
		case ItemSpecial::MaximumMana:
		case ItemSpecial::MaximumStamina:
		case ItemSpecial::MaximumWeight:
		case ItemSpecial::BlueMagicDefense:
		case ItemSpecial::WhiteMagicDefense:
		case ItemSpecial::BlackMagicDefense:
		case ItemSpecial::StatModifierHealth:
		case ItemSpecial::StatModifierMana:
		case ItemSpecial::StatModifierStamina:
		case ItemSpecial::StatModifierWeight:
		case ItemSpecial::StaminaRecover:
			special.append(boost::str(boost::format(ItemSpecial::ToString(item.GetSpecialId(i))) % item.GetSpecialOption(i, 0)));
			break;
		default:
			special.append(boost::str(boost::format(ItemSpecial::ToString(item.GetSpecialId(i)))));
			break;
		}
		special.append("\n");
	}

	special_->setText(special);
	if (!special.empty())
	{
		special_->show();
	}
}

void ItemTooltip::SetRepairEffect(const ItemAttributeComponent& item)
{

}

void ItemTooltip::AppendRepairInfo(const unsigned int duraAfter, const unsigned int cost)
{
	repairInfo_->setText(boost::str(boost::format("Durability after repair: %d\nRepair cost: %d") % duraAfter % cost));

	repairInfo_->show();

	float width  = window_->getPixelSize().d_width;
	float height = window_->getPixelSize().d_height;

	window_->setWidth(UDim(0, 210.f));

	repairInfo_->setPosition(UVector2(UDim(0, 0), UDim(0, height)));

	float repairInfoWidth = PropertyHelper::stringToFloat(repairInfo_->getProperty("HorzExtent"));
	if (repairInfoWidth > width)
	{
		width = repairInfoWidth;
	}

	float repairInfoHeight = PropertyHelper::stringToFloat(repairInfo_->getProperty("VertExtent"));
	height += repairInfoHeight;
	repairInfo_->setHeight(UDim(0, repairInfoHeight));

	window_->setWidth(UDim(0, width+8));
	window_->setHeight(UDim(0, height+4));
}

} // namespace GUI