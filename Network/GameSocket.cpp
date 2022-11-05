
#include "GameSocket.h"
#include "GamePacket.h"
#include <SFML/Network/Socket.hpp>
#include "GameProtocol.h"

namespace Network
{

GameSocket::GameSocket() :
sendCount_(0),
encrypt_(false)
{
}

sf::TcpSocket::Status GameSocket::Send(GamePacket& packet)
{
    // Get the data to send from the packet
    std::size_t size = 0;
    const char* data = packet.OnSend(size, sendCount_, encrypt_, key_);

    // Send the packet data
    return sf::TcpSocket::send(data, size);
}

sf::TcpSocket::Status GameSocket::Receive(GamePacket& packet)
{
	// First make sure the packet is empty
	packet.Clear();

	// Game packets start with 2 bytes.
	// This is the packet start boundary.
	// Have no need for it so will just receive and ignore it.
	sf::Uint16 packetStart = 0;
	std::size_t received = 0;
	if (pendingPacket_.startReceived < sizeof(pendingPacket_.start))
	{
        while (pendingPacket_.startReceived < sizeof(pendingPacket_.start))
        {
            char* data = reinterpret_cast<char*>(&pendingPacket_.start) + pendingPacket_.startReceived;
            sf::TcpSocket::Status status = TcpSocket::receive(data, sizeof(pendingPacket_.start) - pendingPacket_.startReceived, received);
            pendingPacket_.startReceived += received;

            if (status != Done)
                return status;
        }

		packetStart = pendingPacket_.start;
    }
	else
	{
		// Already received the packet start in a previous call
		packetStart = pendingPacket_.start;
	}

	// Now receive the next 2 bytes of the packet
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

	// Now receive the last 2 bytes of the packet
	// This will be the packet end
	sf::Uint16 packetEnd = 0;
	if (pendingPacket_.endReceived < sizeof(pendingPacket_.end))
	{
        while (pendingPacket_.endReceived < sizeof(pendingPacket_.end))
        {
            char* data = reinterpret_cast<char*>(&pendingPacket_.end) + pendingPacket_.endReceived;
            sf::TcpSocket::Status status = TcpSocket::receive(data, sizeof(pendingPacket_.end) - pendingPacket_.endReceived, received);
            pendingPacket_.endReceived += received;

            if (status != Done)
                return status;
        }

		packetEnd = pendingPacket_.end;
    }
	else
	{
		// Already received the packet size in a previous call
		packetEnd = pendingPacket_.end;
	}

    // We have received all the packet data: we can copy it to the user packet
    if (!pendingPacket_.data.empty())
	{
        packet.OnReceive(&pendingPacket_.data[0], pendingPacket_.data.size(), encrypt_, key_);
	}

    // Clear the pending packet data
    pendingPacket_ = PendingPacket();

    return sf::TcpSocket::Done;
}

bool GameSocket::HandleEncryptionStartResult(GamePacket& packet)
{
	sf::Uint8 result = GameProtocol::FAIL;
	packet >> result;

	if (result == GameProtocol::SUCCESS)
	{
		sf::Uint8 keyPart[2][GamePacket::D_XORTABLE_LENGTH];
		for (unsigned int i = 0; i < 2; ++i)
		{
			for (unsigned int j = 0; j < GamePacket::D_XORTABLE_LENGTH; ++j)
			{
				packet >> keyPart[i][j];
			}
		}

		for (unsigned int i = 0; i < GamePacket::D_XORTABLE_LENGTH; ++i)
		{
			key_[i] = keyPart[0][i] ^ keyPart[1][i];
		}

		encrypt_ = true;
	}

	return result == GameProtocol::SUCCESS ? true : false;
}

GameSocket::PendingPacket::PendingPacket() :
start(0),
end(0),
size(0),
startReceived(0),
endReceived(0),
sizeReceived(0),
data()
{
}

} // namespace Network