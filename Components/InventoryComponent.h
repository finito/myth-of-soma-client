#ifndef COMPONENTS_INVENTORY_COMPONENT_H
#define COMPONENTS_INVENTORY_COMPONENT_H

#include "Artemis/Component.h"
#include "Artemis/Entity.h"
#include <string>
#include <vector>

#include "Components/ItemAttributeComponent.h"

namespace Network
{
	class GamePacket;
}

class MagicData;
class SpecialAttackData;
class AbilityData;

typedef boost::shared_ptr<MagicData> MagicDataPtr;
typedef boost::shared_ptr<SpecialAttackData> SpecialAttackDataPtr;
typedef boost::shared_ptr<AbilityData> AbilityDataPtr;

class InventoryComponent : public artemis::Component
{
public:
	InventoryComponent();

	void AddMagic(Network::GamePacket& packet);
	void ClearMagic();

	void AddSpecialAttack(Network::GamePacket& packet);
	void ClearSpecialAttack();

	void AddAbility(Network::GamePacket& packet);
	void ClearAbility();

	ItemAttributeComponent& GetInventoryItem(int index);
	std::size_t GetInventoryItemSize() const;

	ItemAttributeComponent& GetBeltItem(int index);
	std::size_t GetBeltItemSize() const;

	void ClearInventoryItem();
	void ClearBeltItem();

	void ResizeInventoryItem(std::size_t size);
	void ResizeBeltItem(std::size_t size);

	SpecialAttackDataPtr GetSpecialAttackById(short id) const;
	SpecialAttackDataPtr GetSpecialAttackByIndex(int index) const;
	std::size_t GetSpecialAttackSize() const;

	MagicDataPtr GetMagic(short id) const;
	std::size_t GetMagicSize() const;

	AbilityDataPtr GetAbility(short id) const;
	std::size_t GetAbilitySize() const;

	MagicDataPtr GetBlueMagic(int index) const;
	std::size_t GetBlueMagicSize() const;

	MagicDataPtr GetWhiteMagic(int index) const;
	std::size_t GetWhiteMagicSize() const;

	MagicDataPtr GetBlackMagic(int index) const;
	std::size_t GetBlackMagicSize() const;

private:
	// Inventory and Belt
	typedef std::vector<ItemAttributeComponent> ItemArray;
	ItemArray inventoryItem_;
	ItemArray beltItem_;

	// Magic
	typedef std::map<int, MagicDataPtr>  MagicMap;
	typedef std::vector<MagicDataPtr>	 BlueMagic;
	typedef std::vector<MagicDataPtr>	 WhiteMagic;
	typedef std::vector<MagicDataPtr>	 BlackMagic;
	MagicMap   magicMap_;
	BlueMagic  blueMagic_;
	WhiteMagic whiteMagic_;
	BlackMagic blackMagic_;

	// Special Attack
	typedef std::map<int, SpecialAttackDataPtr>  SpecialAttackMap;
	typedef std::vector<SpecialAttackDataPtr>	 SpecialAttack;
	SpecialAttackMap specialAttackMap_;
	SpecialAttack	 specialAttack_;

	// Ability (Crafting, Things like repair...)
	typedef std::map<int, AbilityDataPtr>  AbilityMap;
	typedef std::vector<AbilityDataPtr>	   Ability;
	AbilityMap abilityMap_;
	Ability	   ability_;
};

#endif // COMPONENTS_INVENTORY_COMPONENT_H