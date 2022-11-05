
#include "DataTables/ItemTable.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

void ItemTable::Load(const std::string& filename)
{
	using boost::property_tree::ptree;
	ptree pt;

	read_xml(filename, pt);

	BOOST_FOREACH(ptree::value_type& v, pt.get_child("items"))
	{
		const ptree& child = v.second;
		ItemElement element;
		element.armatureIndex = child.get<unsigned short>("<xmlattr>.armatureIndex");
		element.equipPosition = child.get<unsigned char>("<xmlattr>.equipPosition");
		element.drawOrder = child.get<unsigned char>("<xmlattr>.drawOrder");
		element.devil = child.get<bool>("<xmlattr>.devil");
		items_[child.get<unsigned short>("<xmlattr>.pictureNumber")] = element;
	}
}

const ItemTable::ItemElement* ItemTable::Get(unsigned short pictureId) const
{
	auto it = items_.find(pictureId);
	if (it != items_.end())
	{
		return &(*it).second;
	}
	return nullptr;
}

//unsigned short ItemTable::GetArmatureIndex() const
//{
//	auto it = items_.find()
//}
//
//unsigned char ItemTable::GetEquipPosition() const
//{
//
//
//}
//unsigned char ItemTable::GetDrawOrder() const
//{
//
//}
//bool ItemTable::IsDevil() const
//{
//
//}