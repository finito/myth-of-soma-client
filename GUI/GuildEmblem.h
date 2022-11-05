#ifndef GUI_GUILD_EMBLEM_H
#define GUI_GUILD_EMBLEM_H

#include <CEGUI/CEGUI.h>

namespace Network
{
	class GamePacket;
}

namespace GUI
{

class GuildEmblem
{
public:
	static const int GUILD_EMBLEM_BMP_HEADER_SIZE = 76;
	static const int GUILD_EMBLEM_16BMP_DATA_SIZE = 224 * 2;
	static const int GUILD_EMBLEM_32BMP_DATA_SIZE = 224 * 4;

	struct BMP16_HEADER
	{
		char id[4];
		char remark[64];
		int	width;
		int height;
	};

	~GuildEmblem();

	CEGUI::String GetImageAsString(short guildId, unsigned short guildPictureId);

	void HandleGuildEmblem(Network::GamePacket& packet);

private:
	void Save(short guildId);
	bool Load(short guildId, unsigned short guildPictureId);
	CEGUI::Imageset* CreateImageset(short guildId, char* textureBuffer);
	void SendGuildEmblemRequest(short guildId);

private:
	struct GuildEmblemData
	{
		unsigned short guildPictureId;
		bool loaded;
		CEGUI::Imageset* imageset;
	};

	std::map<unsigned int, GuildEmblemData> emblems_;
};

} // namespace GUI

#endif // GUI_GUILD_EMBLEM_H