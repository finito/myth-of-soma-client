#ifndef SESSION_SOCKET_H
#define SESSION_SOCKET_H

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Clock.hpp>

namespace Network
{

class SessionPacket;

class SessionSocket : public sf::TcpSocket
{
public:
	// Send a session formatted packet of data
	 sf::TcpSocket::Status Send(SessionPacket& packet);

	// Receive a session formatted packet of data
	 sf::TcpSocket::Status Receive(SessionPacket& packet);

	 // Keep the connection alive by sending packet to session server
	 void SendKeepAlive();

private:
	struct PendingPacket
	{
		PendingPacket();

		sf::Uint16		  id;		     // The protocol message identifier
		sf::Uint16		  size;	     // Size of the packet data (is the data only excludes size, packet start and end, and version)
		std::size_t		  idReceived;   // Number of bytes received so far for identifier
		std::size_t		  sizeReceived; // Number of bytes received so far for data size
		std::vector<char> data;	     // The packet data
	};

	PendingPacket pendingPacket_; // Temporary data of the packet currently being received
	sf::Clock	  keepAliveTimer_;
};

} // namespace Network

#endif // SESSION_SOCKET_H