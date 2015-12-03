#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL_net.h>
#include <memory>
#include "../shared/BasePacket.h"

class Client
{
public:
	Client( Uint8 ID, TCPsocket tcpsock, UDPsocket udpsock );
	~Client();

	//TODO: handle errors
	void UDPSend( const BasePacket& packet );
	//TODO: handle errors
	void TCPSend( std::unique_ptr<BasePacket> packet );

	// set the destination for UDP packets bound for this client
	void SetUDPAddress( IPaddress address );

	TCPsocket GetTCPSocket() { return TCPsocket_; }
	Uint8 GetID() { return ID_; }
	bool NeedsUDPAddress() { return !hasUDPAddress_; }

private:
	TCPsocket TCPsocket_;
	UDPsocket UDPsocket_;
	UDPpacket UDPpacket_;
	bool hasUDPAddress_;
	Uint8 ID_;
};

#endif
