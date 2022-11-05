
#include "SessionPacket.h"

namespace Network
{

SessionPacket::SessionPacket() :
readPos_(0),
isValid_(true)
{
}

SessionPacket::SessionPacket(const sf::Uint16 id) :
readPos_(0),
isValid_(true)
{
	data_.resize(4, 0);
	std::memcpy(&data_[0], &id, sizeof(id));
}

void SessionPacket::Clear()
{
    data_.clear();
    readPos_ = 0;
    isValid_ = true;
}

void SessionPacket::Append(const void* data, const std::size_t sizeInBytes)
{
    if (data && (sizeInBytes > 0))
    {
        const std::size_t start = data_.size();
        data_.resize(start + sizeInBytes);
        std::memcpy(&data_[start], data, sizeInBytes);
    }
}

const char* SessionPacket::GetData() const
{
    return !data_.empty() ? &data_[0] : nullptr;
}

std::size_t SessionPacket::GetDataSize() const
{
    return data_.size();
}

bool SessionPacket::EndOfPacket() const
{
    return readPos_ >= data_.size();
}

SessionPacket::operator bool() const
{
    return isValid_;
}

SessionPacket& SessionPacket::operator >>(bool& data)
{
    sf::Uint8 value;
    if (*this >> value)
        data = (value != 0);

    return *this;
}

SessionPacket& SessionPacket::operator >>(sf::Int8& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Int8*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

SessionPacket& SessionPacket::operator >>(sf::Uint8& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Uint8*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

SessionPacket& SessionPacket::operator >>(sf::Int16& data)
{
    if (CheckSize(sizeof(data)))
    {
		data = *reinterpret_cast<const sf::Int16*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

SessionPacket& SessionPacket::operator >>(sf::Uint16& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Uint16*>(GetData() + readPos_);
       readPos_ += sizeof(data);
    }

    return *this;
}

SessionPacket& SessionPacket::operator >>(sf::Int32& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Int32*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

SessionPacket& SessionPacket::operator >>(sf::Uint32& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const sf::Uint32*>(GetData() + readPos_);
		readPos_ += sizeof(data);
    }

    return *this;
}

SessionPacket& SessionPacket::operator >>(float& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const float*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

SessionPacket& SessionPacket::operator >>(double& data)
{
    if (CheckSize(sizeof(data)))
    {
        data = *reinterpret_cast<const double*>(GetData() + readPos_);
        readPos_ += sizeof(data);
    }

    return *this;
}

SessionPacket& SessionPacket::operator >>(char* data)
{
    // First extract string length
    sf::Uint16 length = 0;
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

SessionPacket& SessionPacket::operator >>(std::string& data)
{
    // First extract string length
    sf::Uint16 length = 0;
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

SessionPacket& SessionPacket::operator <<(const bool data)
{
    *this << static_cast<sf::Uint8>(data);
    return *this;
}

SessionPacket& SessionPacket::operator <<(const sf::Int8 data)
{
    Append(&data, sizeof(data));
    return *this;
}

SessionPacket& SessionPacket::operator <<(const sf::Uint8 data)
{
    Append(&data, sizeof(data));
    return *this;
}

SessionPacket& SessionPacket::operator <<(const sf::Int16 data)
{
    sf::Int16 toWrite = data;
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

SessionPacket& SessionPacket::operator <<(const sf::Uint16 data)
{
    sf::Uint16 toWrite = data;
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

SessionPacket& SessionPacket::operator <<(const sf::Int32 data)
{
    sf::Int32 toWrite = data;
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

SessionPacket& SessionPacket::operator <<(const sf::Uint32 data)
{
    sf::Uint32 toWrite = data;
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

SessionPacket& SessionPacket::operator <<(const float data)
{
    Append(&data, sizeof(data));
    return *this;
}

SessionPacket& SessionPacket::operator <<(const double data)
{
    Append(&data, sizeof(data));
    return *this;
}

SessionPacket& SessionPacket::operator <<(const char* data)
{
    // First insert string length
    sf::Uint16 length = 0;
    for (const char* c = data; *c != '\0'; ++c)
        ++length;
    *this << length;

    // Then insert characters
    Append(data, length * sizeof(char));

    return *this;
}

SessionPacket& SessionPacket::operator <<(const std::string& data)
{
    // First insert string length
    sf::Uint16 length = static_cast<sf::Uint16>(data.size());
    *this << length;

    // Then insert characters
    if (length > 0)
    {
        Append(data.c_str(), length * sizeof(std::string::value_type));
    }

    return *this;
}

bool SessionPacket::CheckSize(const std::size_t size)
{
   isValid_ = isValid_ && (readPos_ + size <= data_.size());

    return isValid_;
}

const char* SessionPacket::OnSend(std::size_t& size)
{
	size = GetDataSize();

	sf::Uint16 packetSize = static_cast<sf::Uint16>(size) - 4;
	std::memcpy(&data_[2], &packetSize, sizeof(packetSize));

    return GetData();
}

void SessionPacket::OnReceive(const char* data, const std::size_t size)
{
    Append(data, size);
}

} // namespace Network