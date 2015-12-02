#include "Client.h"

Client::Client( Uint8 ID, TCPsocket tcpsock ) :
ID_( ID ),
TCPsocket_( tcpsock ),
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

void Client::UDPSend( std::unique_ptr<BasePacket> packet )
{

}

void Client::SetUDPAddress( IPaddress address )
{
	UDPpacket_.address = address;
}