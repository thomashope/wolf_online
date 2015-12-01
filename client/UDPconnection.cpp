#include "UDPconnection.h"
#include "../shared/BasePacket.h"

UDPConnection::UDPConnection()
{
}

UDPConnection::~UDPConnection()
{
}

bool UDPConnection::Connect( std::string host, Uint16 port )
{
	// Resolve server name
	if( SDLNet_ResolveHost( &address_, host.c_str(), port ) == -1 )
	{
		fprintf( stderr, "SDLNet_ResolveHost(%s %d): %s\n", host.c_str( ), port, SDLNet_GetError( ) );
		exit( EXIT_FAILURE );
	}
	// Open a socket on random port
	if( !(socket_ = SDLNet_UDP_Open( 0 )) )
	{
		fprintf( stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError( ) );
		exit( EXIT_FAILURE );
	}
	//*
	// Allocate memory for the packet
	//if( !(UDP_send_packet = SDLNet_AllocPacket( 128 )) )
	//{
	//	fprintf( stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError( ) );
	//	exit( EXIT_FAILURE );
	//}

	// Setup the packet
	//SDLpacket_.address.host = address_.host;	// Set the destination host
	//SDLpacket_.address.port = address_.port;	// Set the destination port
	//SDLpacket_.len = UNIVERSAL_PACKET_SIZE;		// Packet size in bytes
	//SDLpacket_.maxlen = UNIVERSAL_PACKET_SIZE;
	//SDLpacket_.data = packet_.Data();			// use the buffer in the universal packet
}

void UDPConnection::SendPacket( BasePacket* packet )
{
	UDPpacket SDLpacket;

	// setup the packet
	SDLpacket.address.host = address_.host;
	SDLpacket.address.port = address_.port;
	SDLpacket.data = packet->Data();
	SDLpacket.len = packet->Size();
	SDLpacket.maxlen = packet->Size();

	std::cout << "sending UDP packet" << std::endl;
	SDLNet_UDP_Send( socket_, -1, &SDLpacket );
	std::cout << "done sending UDP packet" << std::endl;
}