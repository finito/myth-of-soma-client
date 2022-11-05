
#include "GamePacket.h"

namespace Network
{

GamePacket::GamePacket() :
readPos_(0),
isValid_(true)
{
}

GamePacket::GamePacket(const sf::Uint8 id, const bool encrypted) :
readPos_(0),
isValid_(true)
{
	if (encrypted)
	{
		data_.resize(9, 0);
	}
	else
	{
		data_.resize(4, 0);
	}
	*this << id;
}

void GamePacket::Clear()
{
    data_.clear();
    readPos_ = 0;
    isValid_ = true;
}

void GamePacket::Append(const void* data, const std::size_t sizeInBytes)
{
    if (data && (sizeInBytes > 0))
    {
        const std::size_t start = data_.size();
        data_.resize(start + sizeInBytes);
        std::memcpy(&data_[start], data, sizeInBytes);
    }
}

void GamePacket::AppendString(const char* data)
{
    // First insert string length
    sf::Uint8 length = 0;
    for (const char* c = data; *c != '\0'; ++c)
        ++length;

    // Then insert characters
    Append(data, length * sizeof(char));
}

void GamePacket::AppendString(const std::string& data)
{
    // First insert string length
    sf::Uint8 length = static_cast<sf::Uint8>(data.size());

    // Then insert characters
    if (length > 0)
    {
        Append(data.c_str(), length * sizeof(std::string::value_type));
    }
}

void GamePacket::RetrieveString(std::string& data, const sf::Uint16 length)
{
    data.clear();
    if ((length > 0) && CheckSize(length))
    {
        // Then extract characters
        data.assign(GetData() + readPos_, length);

        // Update reading position
        readPos_ += length;
    }
}

const char* GamePacket::GetData() const
{
    return !data_.empty() ? &data_[0] : nullptr;
}

std::size_t GamePacket::GetDataSize() const
{
    return data_.size();
}

bool GamePacket::EndOfPacket() const
{
    return readPos_ >= data_.size();
}

GamePacket::operator bool() const
{
    return isValid_;
}

GamePacket& GamePacket::operator >>(bool& data)
{
    sf::Uint8 value;
    if (*this >> value)
        data = (value != 0);

    return *this;
}

GamePacket& GamePacket::operator >>(sf::Int8& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Int8*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

GamePacket& GamePacket::operator >>(sf::Uint8& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Uint8*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

GamePacket& GamePacket::operator >>(sf::Int16& data)
{
    if (CheckSize(sizeof(data)))
    {
		data = *reinterpret_cast<const sf::Int16*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

GamePacket& GamePacket::operator >>(sf::Uint16& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Uint16*>(GetData() + readPos_);
       readPos_ += sizeof(data);
    }

    return *this;
}

GamePacket& GamePacket::operator >>(sf::Int32& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Int32*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

GamePacket& GamePacket::operator >>(sf::Uint32& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Uint32*>(GetData() + readPos_);
		readPos_ += sizeof(data);
    }

    return *this;
}

GamePacket& GamePacket::operator >>(float& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const float*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

GamePacket& GamePacket::operator >>(double& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const double*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

GamePacket& GamePacket::operator >>(char* data)
{
    // First extract string length
    sf::Uint8 length = 0;
    *this >> length;

    if ((length > 0) && CheckSize(length))
    {
        // Then extract characters
        std::memcpy(data, GetData() + readPos_, length);
        data[length] = '\0';

        // Update reading position
        readPos_ += length;
    }

    return *this;
}

GamePacket& GamePacket::operator >>(std::string& data)
{
    // First extract string length
    sf::Uint8 length = 0;
    *this >> length;

    data.clear();
    if ((length > 0) && CheckSize(length))
    {
        // Then extract characters
        data.assign(GetData() + readPos_, length);

        // Update reading position
        readPos_ += length;
    }

    return *this;
}

GamePacket& GamePacket::operator <<(const bool data)
{
    *this << static_cast<sf::Uint8>(data);
    return *this;
}

GamePacket& GamePacket::operator <<(const sf::Int8 data)
{
    Append(&data, sizeof(data));
    return *this;
}

GamePacket& GamePacket::operator <<(const sf::Uint8 data)
{
    Append(&data, sizeof(data));
    return *this;
}

GamePacket& GamePacket::operator <<(const sf::Int16 data)
{
    sf::Int16 toWrite = data;
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

GamePacket& GamePacket::operator <<(const sf::Uint16 data)
{
    sf::Uint16 toWrite = data;
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

GamePacket& GamePacket::operator <<(const sf::Int32 data)
{
    sf::Int32 toWrite = data;
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

GamePacket& GamePacket::operator <<(const sf::Uint32 data)
{
    sf::Uint32 toWrite = data;
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

GamePacket& GamePacket::operator <<(const float data)
{
    Append(&data, sizeof(data));
    return *this;
}

GamePacket& GamePacket::operator <<(double data)
{
    Append(&data, sizeof(data));
    return *this;
}

GamePacket& GamePacket::operator <<(const char* data)
{
    // First insert string length
    sf::Uint8 length = 0;
    for (const char* c = data; *c != '\0'; ++c)
        ++length;
	*this << length;

    // Then insert characters
    Append(data, length * sizeof(char));

    return *this;
}

GamePacket& GamePacket::operator <<(const std::string& data)
{
    // First insert string length
    sf::Uint8 length = static_cast<sf::Uint8>(data.size());
	*this << length;

    // Then insert characters
    if (length > 0)
    {
        Append(data.c_str(), length * sizeof(std::string::value_type));
    }

    return *this;
}

bool GamePacket::CheckSize(const std::size_t size)
{
   isValid_ = isValid_ && (readPos_ + size <= data_.size());

    return isValid_;
}

const char* GamePacket::OnSend(std::size_t& size, sf::Uint32& sendCount, const bool encrypt, unsigned char* key)
{
	size = GetDataSize(); 

	std::memcpy(&data_[0], &PACKET_START, sizeof(PACKET_START));

	sf::Uint16 packetSize = static_cast<sf::Uint16>(size) - 4;
	std::memcpy(&data_[2], &packetSize, sizeof(packetSize));	

	if (encrypt)
	{
		++sendCount;
		sendCount &= 0x00FFFFFF;
		std::memcpy(&data_[4], &sendCount, sizeof(sendCount));
#ifdef CONNECT_TO_TEST_SERVER
		const sf::Uint8 version = 0x10;
#else
		const sf::Uint8 version = 0xDF;
#endif
		std::memcpy(&data_[8], &version, sizeof(version));
		EncodeDecode(key, packetSize);
	}

	*this << PACKET_END;
	size += 2; // Adding the 2 because of packet end

    return GetData();
}

void GamePacket::OnReceive(const char* data, std::size_t size, bool encrypted, unsigned char* key)
{
	Append(data, size);

	if (encrypted)
	{
		EncodeDecode(key, size, 0);
	}
}

void GamePacket::EncodeDecode(unsigned char* key, const sf::Uint16 size, const sf::Uint16 offset)
{
	const unsigned char l_BYTE_Xor1 = size * D_WORD_XOR1MUL;
	unsigned char l_BYTE_Xor3;
	unsigned short l_WORD_Xor3 = D_WORD_XOR3BASE;

	for (sf::Uint16 i = 0; i < size; i++)
	{
		l_BYTE_Xor3  = static_cast<unsigned char>((l_WORD_Xor3 >> 8) & 0xFF);
		data_[i+offset] = data_[i+offset] ^ l_BYTE_Xor1 ^ key[i % D_XORTABLE_LENGTH] ^ l_BYTE_Xor3;
		l_WORD_Xor3 *= D_WORD_XOR3MUL;
	}
}

} // namespace Network