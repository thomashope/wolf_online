#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL_net.h>
#include <memory>
#include "../shared/BasePacket.h"
#include "../shared/vec2.h"

class Client
{
public:
	Client( Uint8 ID, TCPsocket tcpsock, UDPsocket udpsock );
	~Client();

	//TODO: handle errors
	void UDPSend( const BasePacket& packet );
	//TODO: handle errors
	void TCPSend( const BasePacket& packet );

	// set the destination for UDP packets bound for this client
	void SetUDPAddress( IPaddress address );

	//
	void SetPosition( Vec2 position ) { position_ = position; }

	// Allows the clients to manage its own socket in the set
	void AttachSocketSet( SDLNet_SocketSet* sockset ) { SockSet_ = sockset; }

	TCPsocket GetTCPSocket() { return TCPsocket_; }
	Uint8 GetID() { return ID_; }
	Vec2 GetPosition() { return position_; }

	bool NeedsUDPAddress() { return !hasUDPAddress_; }

private:
	TCPsocket TCPsocket_;
	UDPsocket UDPsocket_;
	UDPpacket UDPpacket_;
	bool hasUDPAddress_;
	Uint8 ID_;

	SDLNet_SocketSet* SockSet_;

	Vec2 position_;
};

#endif
