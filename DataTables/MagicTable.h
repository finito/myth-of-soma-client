#ifndef DATA_TABLES_MAGIC_TABLE_H
#define DATA_TABLES_MAGIC_TABLE_H

#include <string>
#include <map>

class MagicTable
{
public:
	struct MagicElement
	{
		unsigned short magicId;
		int attackType;
		short method;
		int targetType;
		unsigned int startPreMagic;
		unsigned int startPostMagic;
		int startTime;
		unsigned int flyMagic;
		int effectKind;
		int fireNumber;
		unsigned int endPreMagic;
		unsigned int endPostMagic;
		int continueType;
		int continueTime;
	};

	void Load(const std::string& filename);

	const MagicElement* Get(unsigned short magicId) const;
private:
	std::map<unsigned int, MagicElement> magics_;
};

#endif // DATA_TABLES_MAGIC_TABLE_H