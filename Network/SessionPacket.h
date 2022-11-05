#ifndef SESSION_PACKET_H
#define SESSION_PACKET_H

#include <SFML/Config.hpp>
#include <string>
#include <vector>

namespace Network
{

class SessionSocket;

// Implementation of a session packet, so can communicate with
// soma session services.
// This is heavily based upon sf::Packet

class SessionPacket
{
public:
	// Creates an empty packet
	SessionPacket();

	// Creates a packet with the given identifier
	explicit SessionPacket(sf::Uint16 id);

	// Clears the packet, will be empty after
	void Clear();

	// Append data to the end of the packet
	void Append(const void* data, std::size_t sizeInBytes);

	// Get a pointer to the packet data
	const char* GetData() const;

	// Get the size of the packet data
	std::size_t GetDataSize() const;

	// Tell if the reading position has reached the end of the packet
	bool EndOfPacket() const;

	// Test the validity of the packet, for reading
	operator bool() const;

	// Overloads of operator >> to read data from the packet
    SessionPacket& operator >>(bool&         data);
    SessionPacket& operator >>(sf::Int8&     data);
    SessionPacket& operator >>(sf::Uint8&    data);
    SessionPacket& operator >>(sf::Int16&    data);
    SessionPacket& operator >>(sf::Uint16&   data);
    SessionPacket& operator >>(sf::Int32&    data);
    SessionPacket& operator >>(sf::Uint32&   data);
    SessionPacket& operator >>(float&        data);
    SessionPacket& operator >>(double&       data);
    SessionPacket& operator >>(char*         data);
    SessionPacket& operator >>(std::string&  data);

	// Overloads of operator << to write data into the packet
    SessionPacket& operator <<(bool                data);
    SessionPacket& operator <<(sf::Int8            data);
    SessionPacket& operator <<(sf::Uint8           data);
    SessionPacket& operator <<(sf::Int16           data);
    SessionPacket& operator <<(sf::Uint16          data);
    SessionPacket& operator <<(sf::Int32           data);
    SessionPacket& operator <<(sf::Uint32          data);
    SessionPacket& operator <<(float               data);
    SessionPacket& operator <<(double              data);
    SessionPacket& operator <<(const char*         data);
    SessionPacket& operator <<(const std::string&  data);

private:
	friend class SessionSocket;

	// Check if the packet can extract a given number of bytes
	// This function updates accordingly the state of the packet.
	bool CheckSize(std::size_t size);

	// Called before the packet is sent over the network
	// Will be used to do any needed transform of the packet data.
	const char* OnSend(std::size_t& size);

	// Called after the packet is received over the network
	// Will be used to do any needed transform of the packet data.
	void OnReceive(const char* data, std::size_t size);

	std::vector<char> data_; // The packet contents
	std::size_t		  readPos_; // Current read position within the packet
	bool			  isValid_; // The read state of the packet
};

} // namespace Network

#endif // SESSION_PACKET_H