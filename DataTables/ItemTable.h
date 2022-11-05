#ifndef DATA_TABLES_ITEM_TABLE_H
#define DATA_TABLES_ITEM_TABLE_H

#include <string>
#include <map>

namespace WeaponType
{
	enum Enum
	{
		PunchOrKnuckle,
		Sword,
		BigSword,
		Spear,
		Bow,
		BigAxe,
		Staff,
		CrossBow,
		// There is a gap because there was a value 8 but it is no longer used.
		Axe = 9
	};
}

class ItemTable
{
public:
	struct ItemElement
	{
		unsigned short armatureIndex;
		unsigned char equipPosition;
		unsigned char drawOrder;
		bool devil;
	};

	void Load(const std::string& filename);

	const ItemElement* Get(unsigned short pictureId) const;
private:
	std::map<unsigned short, ItemElement> items_;
};

#endif // DATA_TABLES_ITEM_TABLE_H