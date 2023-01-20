#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <chrono>
#include <SDL_net.h>
#undef main // removes SDLs evil define

#include "../shared/UniversalPacket.h"
#include "Client.h"
#include "tcp.h"
#include "udp.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1177
#define MAX_CLIENTS 8

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
	1, 3, 3, 3, 3, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 4, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

std::vector< std::unique_ptr<Client> > clients;

IPaddress address;
TCPsocket TCP_socket;
SDLNet_SocketSet TCP_SocketSet;
UDPsocket UDP_socket;
UDPpacket UDP_packet;
UniversalPacket uniPacket;

// initialised libraries, opens udp and tcp sockets, sets up packets
void init();

// checks if any new clients want to join and adds them to the list
void accept_client();

Uint8 get_availible_id();
Client* get_client( Uint8 ID );
void DisconnectClient( Uint8 ID );

int main( int argc, char* argv[] )
{
	init();

	/* Main loop */
	bool quit = false;
	while( !quit )
	{
		// Check to see if any sockets are ready
		SDLNet_CheckSockets( TCP_SocketSet, 0 );

		// try to accept new clients
		accept_client();

		// read and respond udp packets
		process_udp();

		// read and respond tcp packets
		process_tcp();
	}

	// cleanup
	SDLNet_FreeSocketSet( TCP_SocketSet );
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}

void init()
{
	std::cout << "Starting wolf_online server..." << std::endl;
	// Initalise SDL
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << "SDL_Init: %s\n" << SDL_GetError() << std::endl;
		exit( 1 );
	}
	// Initalise SDLNet
	if( SDLNet_Init( ) == -1 )
	{
		std::cout << "SDLNet_Init: %s\n" << SDLNet_GetError() << std::endl;
		exit( 2 );
	}
	// Resolve Host
	if( SDLNet_ResolveHost( &address, NULL, SERVERPORT ) == -1 ) {
		printf( "SDLNet_ResolveHost: %s\n", SDLNet_GetError( ) );
		exit( 3 );
	}
	// Open TCP socket
	TCP_socket = SDLNet_TCP_Open( &address );
	if( !TCP_socket ) {
		printf( "SDLNet_TCP_Open: %s\n", SDLNet_GetError( ) );
		exit( 4 );
	}
	// Open UDP socket
	if( !(UDP_socket = SDLNet_UDP_Open( SERVERPORT )) )
	{
		fprintf( stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError( ) );
		exit( 5 );
	}

	std::cout << "Server listening on port: " << SERVERPORT << std::endl;

	// Intertwine the SDLNet packet and the UniversalPacket
	// This allows us to use the SDL funcitons with custom data handling
	UDP_packet.data = uniPacket.Data( );
	UDP_packet.maxlen = uniPacket.Size( );

	// Allocate the socket set
	TCP_SocketSet = SDLNet_AllocSocketSet( MAX_CLIENTS + 1 );
	if( TCP_SocketSet == NULL )
	{
		fprintf( stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError( ) );
		exit( 6 );
	}

	// Add the servers listening socket to the set
	SDLNet_TCP_AddSocket( TCP_SocketSet, TCP_socket );
	SDLNet_CheckSockets( TCP_SocketSet, 0 );
}

void accept_client()
{
	TCPsocket new_socket;

	// try to accept a new connection
	// if there was no connection accept returns null
	if( SDLNet_SocketReady( TCP_socket ) )
	{
		new_socket = SDLNet_TCP_Accept(TCP_socket);
	}
	else return;

	if( new_socket != NULL )
	{
		JoinRequestPacket joinRequest;

		// check for a join request
		if (SDLNet_TCP_Recv(new_socket, joinRequest.Data(), joinRequest.Size()) <= 0)
		{
			// an error occured, set the socket to null and try connecting again
			return;
		}

		// if there is space on the server
		if( clients.size() < MAX_CLIENTS )
		{
			// respon with OK
			JoinResponsePacket response;
			response.SetResponse(JR_OK);

			response.SetGivenID( get_availible_id() );

			if( SDLNet_TCP_Send( new_socket, response.Data(), response.Size() ) < (int)response.Size() )
			{
				// the whole packet could not be sent, some error occured
				return;
			}

			// add the new client to the vector
			clients.push_back( std::unique_ptr<Client>(new Client( response.GetGivenID(), new_socket, UDP_socket )) );
			clients.back()->SetPosition( joinRequest.GetPosition() );

			// Add the new client to the socket set
			SDLNet_TCP_AddSocket( TCP_SocketSet, clients.back()->GetTCPSocket() );
			SDLNet_CheckSockets( TCP_SocketSet, 0 );
			clients.back()->AttachSocketSet( &TCP_SocketSet );

			// Tell other clients new client joined
			PlayerJoinedPacket playerJoined( clients.back()->GetID() );
			playerJoined.SetPosition( joinRequest.GetPosition() );
			tcp_send_all_except( playerJoined.GetID(), playerJoined );

			std::cout << "Client joined with ID: " << (int)clients.back()->GetID() << std::endl;
		}
		else
		{
			// else reject the new client
			JoinResponsePacket response;
			response.SetResponse( JR_REJECT );

			if( SDLNet_TCP_Send( new_socket, response.Data(), response.Size() ) < (int)response.Size() )
			{
				// the whole packet could not be sent, some error occured
				return;
			}
		}
	}
}

Uint8 get_availible_id()
{
	Uint8 id = 1;
	// keep looping until there is no client with that ID
	while( get_client(id)  )
	{
		id++;
		if( id == 0 )
		{
			// if the Uint8 wraps around to zero, give up and return
			return id;
		}
	}
	return id;
}

Client* get_client( Uint8 ID )
{
	for( auto& client : clients)
	{
		if( client->GetID() == ID)
		{
			return client.get();
		}
	}
	return nullptr;
}

void DisconnectClient( Uint8 ID )
{
	bool found_client = false;

	// remove the client from the vector
	for( auto it = clients.begin(); it != clients.end(); it++ )
	{
		if( (*it)->GetID() == ID )
		{
			clients.erase( it );
			found_client = true;
			break;
		}
	}

	// tell everyone else they disconnected
	if( found_client )
	{
		PlayerDisconnectedPacket disconnect( ID );
		udp_send_all( disconnect );
	}
}


#include "udp.inl"
#include "tcp.inl"