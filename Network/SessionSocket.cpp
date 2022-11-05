
#include "SessionSocket.h"
#include "SessionPacket.h"
#include <SFML/Network/Socket.hpp>
#include "SessionProtocol.h"

namespace Network
{

sf::TcpSocket::Status SessionSocket::Send(SessionPacket& packet)
{
    // Get the data to send from the packet
    std::size_t size = 0;
    const char* data = packet.OnSend(size);

    // Send the packet data
    return sf::TcpSocket::send(data, size);
}

sf::TcpSocket::Status SessionSocket::Receive(SessionPacket& packet)
{
	// First make sure the packet is empty
	packet.Clear();

	// Session packets start with 4 bytes. This contains the
	// packet identifier and size of the remaining packet data
	// therefore the total size of the packet is the 4 bytes +
	// the 2 bytes that is the size of remaining packet data.


	// Start by receiving the first 2 bytes of the packet
	// This will be the packet id
	sf::Uint16 packetId = 0;
	std::size_t received = 0;
	if (pendingPacket_.idReceived < sizeof(pendingPacket_.id))
	{
        while (pendingPacket_.idReceived < sizeof(pendingPacket_.id))
        {
            char* data = reinterpret_cast<char*>(&pendingPacket_.id) + pendingPacket_.idReceived;
            sf::TcpSocket::Status status = TcpSocket::receive(data, sizeof(pendingPacket_.id) - pendingPacket_.idReceived, received);
            pendingPacket_.idReceived += received;

            if (status != Done)
                return status;
        }

		packetId = pendingPacket_.id;
    }
	else
	{
		// Already received the packet id in a previous call
		packetId = pendingPacket_.id;
	}

	// Now receieve the next 2 bytes of the packet
	// This will be the packet size
	sf::Uint16 packetSize = 0;
	if (pendingPacket_.sizeReceived < sizeof(pendingPacket_.size))
	{
        while (pendingPacket_.sizeReceived < sizeof(pendingPacket_.size))
        {
            char* data = reinterpret_cast<char*>(&pendingPacket_.size) + pendingPacket_.sizeReceived;
            sf::TcpSocket::Status status = TcpSocket::receive(data, sizeof(pendingPacket_.size) - pendingPacket_.sizeReceived, received);
            pendingPacket_.sizeReceived += received;

            if (status != Done)
                return status;
        }

		packetSize = pendingPacket_.size;
    }
	else
	{
		// Already received the packet size in a previous call
		packetSize = pendingPacket_.size;
	}

    // Loop until we receive all the packet data
    char buffer[1024];
    while (pendingPacket_.data.size() < packetSize)
    {
        // Receive a chunk of data
        std::size_t sizeToGet = std::min(static_cast<std::size_t>(packetSize - pendingPacket_.data.size()), sizeof(buffer));
        sf::TcpSocket::Status status = TcpSocket::receive(buffer, sizeToGet, received);
        if (status != Done)
            return status;

        // Append it into the packet
        if (received > 0)
        {
            pendingPacket_.data.resize(pendingPacket_.data.size() + received);
            char* begin = &pendingPacket_.data[0] + pendingPacket_.data.size() - received;
            std::memcpy(begin, buffer, received);
        }
    }

    // We have received all the packet data: we can copy it to the user packet
    if (!pendingPacket_.data.empty())
	{
		// Copy the packetId
		packet.OnReceive(reinterpret_cast<char*>(&packetId), sizeof(packetId));

		// Copy the pending packet data
        packet.OnReceive(&pendingPacket_.data[0], pendingPacket_.data.size());
	}

    // Clear the pending packet data
    pendingPacket_ = PendingPacket();

    return sf::TcpSocket::Done;
}

void SessionSocket::SendKeepAlive()
{
	if (keepAliveTimer_.getElapsedTime().asSeconds() > 15.f)
	{
		SessionPacket packet(SessionProtocol::SM_ALIVE_ACK);
		Send(packet);
		keepAliveTimer_.restart();
	}
}

SessionSocket::PendingPacket::PendingPacket() :
id(0),
size(0),
idReceived(0),
sizeReceived(0),
data()
{
}

} // namespace Network