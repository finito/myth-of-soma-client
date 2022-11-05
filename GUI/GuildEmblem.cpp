
#include "GUI/GuildEmblem.h"

#include "Framework/GUI.h"

#include "Network/GamePacket.h"
#include "Network/GameProtocol.h"
#include "Network/GameSocket.h"

#include "Game.h"

#include <SFML/Graphics/Image.hpp>
#include <boost/format.hpp>
#include <fstream>

namespace GUI
{

using namespace CEGUI;
using namespace Network;

GuildEmblem::~GuildEmblem()
{
	//for (auto it = emblems_.begin(); it != emblems_.end(); ++it)
	//{
	//	System::getSingleton().getRenderer()->destroyTexture(*it->second.imageset->getTexture());
	//	ImagesetManager::getSingleton().destroy(*it->second.imageset);
	//}
}

String GuildEmblem::GetImageAsString(short guildId, unsigned short guildPictureId)
{
	if (guildId == -1)
	{
		return "";
	}

	auto it = emblems_.find(guildId);
	if (it == emblems_.end())
	{
		GuildEmblemData data;
		data.guildPictureId = guildPictureId;
		data.loaded = false;
		data.imageset = nullptr;
		emblems_[guildId] = data;
		if (guildPictureId > 0 && !Load(guildId, guildPictureId))
		{
			SendGuildEmblemRequest(guildId);
		}
		return "";
	}
	else
	{
		if (guildPictureId > 0 && it->second.guildPictureId != guildPictureId)
		{
			it->second.guildPictureId = guildPictureId;
			SendGuildEmblemRequest(guildId);
		}

		if (it->second.loaded)
		{
			return PropertyHelper::imageToString(&it->second.imageset->getImage("Emblem"));
		}
		else
		{
			return "";
		}
	}
}

void GuildEmblem::SendGuildEmblemRequest(short guildId)
{
	GamePacket packet(GameProtocol::PKT_GUILD_SYMBOL_DATA);
	packet << guildId << static_cast<sf::Uint16>(1);
	Game::Instance().gameSocket->Send(packet);
}

void GuildEmblem::HandleGuildEmblem(GamePacket& packet)
{
	sf::Int16 guildId;
	packet >> guildId;

	auto it = emblems_.find(guildId);
	if (it != emblems_.end())
	{
		std::string filename;
		packet >> filename;

		sf::Uint16 emblemPixelLength;
		packet >> emblemPixelLength;

		std::string emblemHeader;
		packet.RetrieveString(emblemHeader,  GUILD_EMBLEM_BMP_HEADER_SIZE);

		std::string emblemPixels;
		packet.RetrieveString(emblemPixels, GUILD_EMBLEM_16BMP_DATA_SIZE);

		// Convert pixel data from 16bpp 565 to 32bpp.
		// Also set the alpha where the colour is bright pink.
		const unsigned short* sourceData = reinterpret_cast<const unsigned short*>(emblemPixels.c_str());
		char buffer[GUILD_EMBLEM_32BMP_DATA_SIZE];
		for (int j = 0 ; j < GUILD_EMBLEM_32BMP_DATA_SIZE; j+=4, ++sourceData)
		{
			unsigned char r = buffer[j] = ((*sourceData & 0xF800) << 3) >> 11;
			unsigned char g = buffer[j+1] = ((*sourceData & 0x07E0) << 2) >> 5;
			unsigned char b = buffer[j+2] = ((*sourceData & 0x001F) << 3);
			buffer[j+3] = (r == 248 && g == 0 && b == 248) ? 0 : 255;
		}

		if (it->second.imageset != nullptr)
		{
			it->second.imageset->getTexture()->loadFromMemory(buffer, Size(16, 14), Texture::PF_RGBA);
		}
		else
		{
			it->second.imageset = CreateImageset(guildId, &buffer[0]);
		}
		it->second.loaded = true;
		Save(guildId);
	}
}

bool GuildEmblem::Load(short guildId, unsigned short guildPictureId)
{
	auto it = emblems_.find(guildId);
	if (it == emblems_.end())
	{
		return false;
	}

	char buffer[GUILD_EMBLEM_32BMP_DATA_SIZE];
	std::ifstream in(boost::str(boost::format("emblems/%1$05i.cache") % guildId), std::ios::binary | std::ios::in);
	if (!in.is_open())
	{
		return false;
	}

	in.read(buffer, GUILD_EMBLEM_32BMP_DATA_SIZE);
	in.read(reinterpret_cast<char*>(&it->second.guildPictureId), sizeof(it->second.guildPictureId));
	if (it->second.guildPictureId != guildPictureId)
	{
		return false;
	}

	it->second.imageset = CreateImageset(guildId, &buffer[0]);
	it->second.loaded = true;
	return true;
}

Imageset* GuildEmblem::CreateImageset(short guildId, char* textureBuffer)
{
	Texture* texture = &Game::Instance().gui->CreateTexture();
	texture->loadFromMemory(textureBuffer, Size(16, 14), Texture::PF_RGBA);

	Imageset* imageset = &ImagesetManager::getSingleton().create(
		"guildemblem" + PropertyHelper::uintToString(guildId), *texture);
	imageset->defineImage("Emblem", CEGUI::Rect(0, 0, 16, 14), CEGUI::Point(0, 0));

	return imageset;
}

void GuildEmblem::Save(short guildId)
{
	auto it = emblems_.find(guildId);
	if (it != emblems_.end() && it->second.imageset != nullptr)
	{
		const size_t bufferSize = GUILD_EMBLEM_32BMP_DATA_SIZE; // data in 0.7.x is always RGBA 32 bit
		char* buffer = new char[bufferSize];

		it->second.imageset->getTexture()->saveToMemory(buffer);

		std::ofstream out(boost::str(boost::format("emblems/%1$05i.cache") % guildId), std::ios::binary | std::ios::out);
		if (out.is_open())
		{
			out.write(buffer, bufferSize);
			out.write(reinterpret_cast<char*>(&it->second.guildPictureId), sizeof(it->second.guildPictureId));
		}
		delete[] buffer;
	}
}

} // namespace GUI