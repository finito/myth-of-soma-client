#ifndef GAME_SOCKET_H
#define GAME_SOCKET_H

#include <SFML/Network/TcpSocket.hpp>

namespace Network
{

class GamePacket;

class GameSocket : public sf::TcpSocket
{
public:
	GameSocket();

	// Send a session formatted packet of data
	 sf::TcpSocket::Status Send(GamePacket& packet);

	// Receive a session formatted packet of data
	 sf::TcpSocket::Status Receive(GamePacket& packet);

	 // Handles the enabling of encryption and setting of the key
	bool HandleEncryptionStartResult(GamePacket& packet);

private:
	struct PendingPacket
	{
		PendingPacket();

		sf::Uint16		  start;		  // Start boundary 0xAA55
		sf::Uint16		  end;			  // End boundary 0x55AA
		sf::Uint16		  size;		  // Size of the packet data (is the data only excludes size, packet start and end, and version)
		std::size_t		  startReceived; // Number of bytes received so far for start boundary
		std::size_t		  sizeReceived;  // Number of bytes received so far for data size
		std::size_t		  endReceived;	  // Number of bytes received so far for end boundary
		std::vector<char> data;		  // The packet data
	};

	PendingPacket pendingPacket_; // Temporary data of the packet currently being received
	sf::Uint32	  sendCount_;	  // A number is given to each packet. The server uses this to help prevent a user repeating the same packet.
	bool		  encrypt_;
	unsigned char key_[8];
};

} // namespace Network

#endif // GAME_SOCKET_H