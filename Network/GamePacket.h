#ifndef GAME_PACKET_H
#define GAME_PACKET_H

#include <SFML/Config.hpp>
#include <string>
#include <vector>

namespace Network
{

class GameSocket;

// Implementation of a game packet, so can communicate with
// soma game server.
// This is heavily based upon sf::Packet

class GamePacket
{
public:
	// Creates an empty packet
	GamePacket();

	// Creates a packet with the given identifier
	explicit GamePacket(sf::Uint8 id, bool encrypted = true);

	// Clears the packet, will be empty after
	void Clear();

	// Append data to the end of the packet
	void Append(const void* data, std::size_t sizeInBytes);

	// Append string to end of the packet without adding length
	void AppendString(const char*);
	void AppendString(const std::string&);

	// Retrieve string from packet, uses different size for length
	// To be used for Retrieving strings from packet that do not use
	// the default single byte for length.
	void RetrieveString(std::string& data, sf::Uint16 length);

	// Get a pointer to the packet data
	const char* GetData() const;

	// Get the size of the packet data
	std::size_t GetDataSize() const;

	// Tell if the reading position has reached the end of the packet
	bool EndOfPacket() const;

	// Test the validity of the packet, for reading
	operator bool() const;

	// Overloads of operator >> to read data from the packet
    GamePacket& operator >>(bool&         data);
    GamePacket& operator >>(sf::Int8&     data);
    GamePacket& operator >>(sf::Uint8&    data);
    GamePacket& operator >>(sf::Int16&    data);
    GamePacket& operator >>(sf::Uint16&   data);
    GamePacket& operator >>(sf::Int32&    data);
    GamePacket& operator >>(sf::Uint32&   data);
    GamePacket& operator >>(float&        data);
    GamePacket& operator >>(double&       data);
    GamePacket& operator >>(char*         data);
    GamePacket& operator >>(std::string&  data);

	// Overloads of operator << to write data into the packet
    GamePacket& operator <<(bool                data);
    GamePacket& operator <<(sf::Int8            data);
    GamePacket& operator <<(sf::Uint8           data);
    GamePacket& operator <<(sf::Int16           data);
    GamePacket& operator <<(sf::Uint16          data);
    GamePacket& operator <<(sf::Int32           data);
    GamePacket& operator <<(sf::Uint32          data);
    GamePacket& operator <<(float               data);
    GamePacket& operator <<(double              data);
    GamePacket& operator <<(const char*         data);
    GamePacket& operator <<(const std::string&  data);

private:
	friend class GameSocket;

	// Boundaries of the game packets
	static const sf::Uint16 PACKET_START = 0x55AA;
	static const sf::Uint16 PACKET_END   = 0xAA55;

	// Encryption and Decryption constants
#ifdef CONNECT_TO_TEST_SERVER
	static const sf::Uint16 D_WORD_XOR1MUL		= 0x004F;
	static const sf::Uint16 D_WORD_XOR3BASE		= 0x5F238;
	static const sf::Uint16 D_WORD_XOR3MUL		= 0x087B;
	static const unsigned int D_XORTABLE_LENGTH	= 8;
#else
	static const sf::Uint16 D_WORD_XOR1MUL		= 0x009D; // 0x004F;
	static const sf::Uint16 D_WORD_XOR3BASE		= 0x086D; // 0xF238;
	static const sf::Uint16 D_WORD_XOR3MUL		= 0x087B;
	static const unsigned int D_XORTABLE_LENGTH	= 8;
#endif

	// Check if the packet can extract a given number of bytes
	// This function updates accordingly the state of the packet.
	bool CheckSize(std::size_t size);

	// Called before the packet is sent over the network
	// Will be used to do any needed transform of the packet data.
	const char* OnSend(std::size_t& size, sf::Uint32& sendCount, bool encrypt, unsigned char* key);

	// Called after the packet is received over the network
	// Will be used to do any needed transform of the packet data.
	void OnReceive(const char* data, std::size_t size, bool encrypted, unsigned char* key);

	// Encryption and Decryption of game packet
	void EncodeDecode(unsigned char* key, sf::Uint16 size, sf::Uint16 offset = 4);

	std::vector<char> data_; // The packet contents
	std::size_t		  readPos_; // Current read position within the packet
	bool			  isValid_; // The read state of the packet
};

} // namespace Network

#endif // GAME_PACKET_H