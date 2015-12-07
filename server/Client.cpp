#include "Client.h"

Client::Client( Uint8 ID, TCPsocket tcpsock, UDPsocket udpsock ) :
ID_( ID ),
TCPsocket_( tcpsock ),
UDPsocket_( udpsock ),
hasUDPAddress_( false )
{
}

Client::~Client()
{
	//TODO: send disconnect message

	SDLNet_TCP_DelSocket( *SockSet_, TCPsocket_ );

	// cleanup the socket
	SDLNet_TCP_Close( TCPsocket_ );
}

void Client::TCPSend( const BasePacket& packet )
{
	SDLNet_TCP_Send( TCPsocket_, packet.Data(), packet.Size() );
}

void Client::UDPSend( const BasePacket& packet )
{
	// if the client hasn't received a udp packet yet, they don't know where to send them
	// drop attempts to send untill the server recvs a udp from them
	if( !hasUDPAddress_ ) return;

	UDPpacket_.data = packet.Data();
	UDPpacket_.len = packet.Size();
	UDPpacket_.maxlen = packet.Size();

	SDLNet_UDP_Send( UDPsocket_, -1, &UDPpacket_ );
}

void Client::SetUDPAddress( IPaddress address )
{
	UDPpacket_.address = address;
	hasUDPAddress_ = true;
}
