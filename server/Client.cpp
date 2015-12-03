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

	// cleanup the socket
	SDLNet_TCP_Close( TCPsocket_ );
}

void Client::TCPSend( std::unique_ptr<BasePacket> packet )
{

}

void Client::UDPSend( const BasePacket& packet )
{
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
