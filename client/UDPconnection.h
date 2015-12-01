#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

#include <SDL2/SDL_net.h>
#include <queue>
#include "../shared/UniversalPacket.h"

class BasePacket;

class UDPConnection {
public:
	UDPConnection();
	~UDPConnection();

	bool Connect( std::string host, Uint16 port );

	// adds a packet to the queue
	void QueuePacket( BasePacket* packet );

	// Create a thread that sends packets as soon as they are added to the queue
	void StartSenderThread();

	void SendPacket( BasePacket* packet );

private:
	IPaddress address_;
	UDPsocket socket_;
	//UDPpacket SDLpacket_;		// TODO: remove this, keep in implementation
	UniversalPacket packet_;

	// Attach to the sender thread
	void SendPackets();
};

#endif