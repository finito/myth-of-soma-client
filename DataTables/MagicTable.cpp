
#include "DataTables/MagicTable.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

void MagicTable::Load(const std::string& filename)
{
	using boost::property_tree::ptree;
	ptree pt;

	read_xml(filename, pt);

	BOOST_FOREACH(ptree::value_type& v, pt.get_child("magics"))
	{
		const ptree& child = v.second;
		MagicElement element;
		element.magicId = child.get<unsigned short>("<xmlattr>.id");
		element.attackType = child.get<int>("<xmlattr>.attackType");
		element.method = child.get<short>("<xmlattr>.method");
		element.targetType = child.get<int>("<xmlattr>.destinationType");
		element.startPreMagic = child.get<unsigned int>("<xmlattr>.startPreMagic");
		element.startPostMagic = child.get<unsigned int>("<xmlattr>.startPostMagic");
		element.startTime = child.get<int>("<xmlattr>.startTime");
		element.flyMagic = child.get<unsigned int>("<xmlattr>.flyMagic");
		element.effectKind = child.get<int>("<xmlattr>.effectKind");
		element.fireNumber = child.get<int>("<xmlattr>.fireNumber");
		element.endPreMagic = child.get<unsigned int>("<xmlattr>.endPreMagic");
		element.endPostMagic = child.get<unsigned int>("<xmlattr>.endPostMagic");
		element.continueType = child.get<int>("<xmlattr>.continueType");
		element.continueTime = child.get<int>("<xmlattr>.continueTime");
		magics_[element.magicId] = element;
	}
}

const MagicTable::MagicElement* MagicTable::Get(unsigned short magicId) const
{
	auto it = magics_.find(magicId);
	if (it != magics_.end())
	{
		return &(*it).second;
	}
	return nullptr;
}