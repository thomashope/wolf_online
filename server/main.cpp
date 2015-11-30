#include <iostream>
#include <iomanip>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#undef main

#include "../../wolf_online_client/src/packets/UniversalPacket.h"
#include "../../wolf_online_client/src/packets/JoinResponsePacket.h"
#include "../../wolf_online_client/src/packets/MapResponsePacket.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1177

#define MAP_WIDTH 24
#define MAP_HEGIHT 24
char mapData[MAP_WIDTH * MAP_HEGIHT] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1,
	1, 3, 3, 3, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 4, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

int main(int argc, char* argv[])
{
	std::cout << "Starting wolf_online server..." << std::endl;

	// initalise libraries
	if( SDL_Init( 0 ) == -1 )
	{
		std::cout << "SDL_Init: %s\n" <<  SDL_GetError() << std::endl;
		exit(1);
	}
	if (SDLNet_Init() == -1)
	{
		std::cout << "SDLNet_Init: %s\n" << SDLNet_GetError() << std::endl;
		exit(2);
	}
		
	UDPsocket UDP_server_socket;
	UDPpacket* UDP_received_packet;
	
	/* Open a socket */
	if( !(UDP_server_socket = SDLNet_UDP_Open( SERVERPORT )) )
	{
		fprintf( stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError() );
		exit( EXIT_FAILURE );
	}
	std::cout << "Server listening on port: " << SERVERPORT << std::endl;
	/* Allocate memory for the packet */
	if( !(UDP_received_packet = SDLNet_AllocPacket( 128 )) )
	{
		fprintf( stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError() );
		exit( EXIT_FAILURE );
	}

	// create a listening TCP socket on port 5555 (server)
	IPaddress address;
	TCPsocket TCP_client_sock = NULL;
	TCPsocket TCP_server_sock;

	if( SDLNet_ResolveHost( &address, NULL, SERVERPORT ) == -1 ) {
		printf( "SDLNet_ResolveHost: %s\n", SDLNet_GetError( ) );
		exit( 4 );
	}
	TCP_server_sock = SDLNet_TCP_Open( &address );
	if( !TCP_server_sock ) {
		printf( "SDLNet_TCP_Open: %s\n", SDLNet_GetError( ) );
		exit( 5 );
	}

	/* Main loop */
	bool quit = false;
	while( !quit )
	{
		/* Wait a packet. UDP_Recv returns != 0 if a packet is coming */
		if( SDLNet_UDP_Recv( UDP_server_socket, UDP_received_packet ) )
		{
			/* print packet info
			std::cout << "UDP Packet incoming\n" << std::endl;
			std::cout << "\tChan:    " << UDP_received_packet->channel << std::endl;
			std::cout << "\tLen:     " << UDP_received_packet->len << std::endl;
			std::cout << "\tMaxlen:  " << UDP_received_packet->maxlen << std::endl;
			std::cout << "\tStatus:  " << UDP_received_packet->status << std::endl;
			std::cout << "\tAddress: " << std::hex
				<< UDP_received_packet->address.host << " " << std::dec
				<< UDP_received_packet->address.port << std::endl;
			//*/
						 
			float fff = Uint32toFloat( SDLNet_Read32( &UDP_received_packet->data[2] ) );

			// print packet data
			std::cout << "\n\tPacket Type: " << (int)UDP_received_packet->data[0]
				<< "\n\tOwnerID: " << (int)UDP_received_packet->data[1]
				<< "\n\tX position: " << Uint32toFloat( SDLNet_Read32( &UDP_received_packet->data[2] ) )
				<< "\n\tY position: " << Uint32toFloat( SDLNet_Read32( &UDP_received_packet->data[6] ) )
				<< std::endl;

			/* Quit if packet contains "quit" */
			if( strcmp( (char *)UDP_received_packet->data, "quit" ) == 0 )
				quit = 1;
		}

		// try to accept a connection if not already accepted
		if( TCP_client_sock == NULL )
		{
			TCP_client_sock = SDLNet_TCP_Accept( TCP_server_sock );
		}
		// if a connection has been made, check it
		if( TCP_client_sock )
		{
			std::cout << "checking TCP" << std::endl;
			//Uint8* buffer = new Uint8[64];
			UniversalPacket packet;

			if( SDLNet_TCP_Recv( TCP_client_sock, packet.Data(), packet.Size() ) <= 0 )
			{
				// an error occured, set the socket to null and try connecting again
				// TODO: find out if this leaves allocated memory?
				TCP_client_sock = NULL;
				continue;
			}

			auto recvd = packet.CreateFromContents();

			// check if its a join request
			if (recvd)
			{
				if (recvd->Type() == PT_JOIN_REQUEST)
				{
					std::cout << "player requested join" << std::endl;

					JoinResponsePacket response;
					response.SetResponse(JR_OK);
					response.SetGivenID(117);

					SDLNet_TCP_Send(TCP_client_sock, response.Data(), response.Size());
				}
				if (recvd->Type() == PT_MAP_REQUEST)
				{
					std::cout << "player asked for map data" << std::endl;

					MapResponsePacket response;
					response.SetMapData(mapData);

					SDLNet_TCP_Send(TCP_client_sock, response.Data(), response.Size());
				}
			}
		}
	}

	// cleanup
	SDLNet_FreePacket( UDP_received_packet );
	SDLNet_Quit( );
	SDL_Quit( );
	return 0;
}