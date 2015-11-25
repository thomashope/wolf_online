#include "TCPConnection.h"
#include "world.h"
#include "JoinRequestPacket.h"
#include "JoinResponsePacket.h"
#include "player.h"

TCPConnection::TCPConnection() :
mapData_( nullptr )
{
	// allocate the buffer
	buffer_ = new Uint8[MAX_BUFFER];
}

TCPConnection::~TCPConnection()
{
	if( mapData_ )
	{
		FreeMapData();
	}

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
	
	JoinRequestPacket request;
	JoinResponsePacket response;

	// Ask to join the server
	SDLNet_TCP_Send( socket_, request.Data(), request.Size() );

	// read the response
	SDLNet_TCP_Recv( socket_, response.Data(), response.Size() );

	if( response.GetResponse() == JR_REJECT )
	{
		// exit if rejected
		std::cout << "You connection was rejected by the server" << std::endl;
		return false;
	}

	player.ID = response.GetGivenID();
	std::cout << "Ther server assigned you an ID of " << player.ID << std::endl;

	// return success
	return true;
}

bool TCPConnection::RequestMapData(World& world)
{
	int width = 0;
	int height = 0;

	// request the data

	// recv the map info

	// recv the map data

	world.SetMap( mapData_, width, height );

	FreeMapData();
	return true;
}

void TCPConnection::FreeMapData()
{
	if( mapData_ )
	{
		delete[] mapData_;
		mapData_ = nullptr;
	}
}