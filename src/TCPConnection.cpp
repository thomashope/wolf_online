#include "TCPConnection.h"
#include "world.h"
#include "packets/JoinRequestPacket.h"
#include "packets/JoinResponsePacket.h"
#include "packets/MapRequestPacket.h"
#include "packets/MapResponsePacket.h"
#include "player.h"

TCPConnection::TCPConnection()
{
	// allocate the buffer
	buffer_ = new Uint8[MAX_BUFFER];
}

TCPConnection::~TCPConnection()
{
	// free the buffer
	delete[] buffer_;
	buffer_ = nullptr;
}

bool TCPConnection::Connect(Player& player, std::string host, Uint16 port)
{
	// Resolve the server name
	if( SDLNet_ResolveHost( &address_, host.c_str(), port ) )
	{
		std::cerr << "SDLNet_ResolveHost(" << host << ", " << port << ") error: "
			<< SDLNet_GetError() << std::endl;
		return false;
	}
	// Establish connection
	socket_ = SDLNet_TCP_Open( &address_ );
	if( !socket_ )
	{
		std::cerr << "SDLNet_TCP_Open error: " << SDLNet_GetError() << std::endl;
		return false;
	}
	
	std::cout << "Established connection, requesting join..." << std::endl;

	JoinRequestPacket request;
	JoinResponsePacket response;

	// Ask to join the server
	SDLNet_TCP_Send( socket_, request.Data(), request.Size() );

	// read the response
	//TODO: check return value for errors
	SDLNet_TCP_Recv( socket_, response.Data(), response.Size() );

	if( response.GetResponse() == JR_REJECT )
	{
		// exit if rejected
		std::cout << "You connection was rejected by the server" << std::endl;
		return false;
	}

	player.ID = response.GetGivenID();
	std::cout << "The server assigned you an ID of " << (int)player.ID << std::endl;

	// return success
	return true;
}

bool TCPConnection::RequestMapData(World& world)
{
	MapRequestPacket request;
	MapResponsePacket response;

	std::cout << "requsting map data" << std::endl;

	// ask the server for the map data
	SDLNet_TCP_Send(socket_, request.Data(), request.Size());

	// recv the whole map ( this blocks untill all the data is received )
	if (SDLNet_TCP_Recv(socket_, response.Data(), response.Size()) < response.Size())
	{
		std::cout << "error recving map data" << std::endl;
		return false;
	}
	
	// construct the world
	world.SetMap( response.GetMapData(), response.Width(), response.Height() );

	return true;
}