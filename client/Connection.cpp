#include "Connection.h"
#include "world.h"
#include "../shared/UniversalPacket.h"

Connection::Connection()
{
}

Connection::~Connection()
{
}

bool Connection::Connect( Player& player, World& world, std::string host, Uint16 port, Uint32& globalTime )
{
	std::cout << "Creating TCP connection" << std::endl;
	if( !tcp_conn_.Connect( player, host, port ) )
		return false;

	if( !udp_conn_.Connect( host, port ) )
		return false;

	// each connection sends packets on a differnt thread as soon as they are avalible
	tcp_conn_.StartSenderThread();
	udp_conn_.StartSenderThread();

	// Ping the server
	if( !SyncTimeWithServer( player, globalTime ) )
		return false;

	// Download the map
	if( !GetMapData( player, world ) )
		return false;
  
	// Get the player list
	TCPSend( new InfoRequestPacket( player.ID, RT_PLAYER_LIST ) );

  return true;
}

bool Connection::SyncTimeWithServer( const Player& player, Uint32& globalTime )
{
	std::cout << "Attempting time sync with server " << std::flush;
	Uint32 sync_start_time = SDL_GetTicks( );
	Uint32 current_time = sync_start_time;
	Uint32 heartbeat_time = 1000;
	std::unique_ptr<BasePacket> recvd;
	while( true )
	{
		if( current_time - sync_start_time > 11000 )
		{
			std::cout << "\nCould not sync time with server" << std::endl;
			return false;
		}

		//  Send a heartbeat packet, wait a bit, then check for a sync
		if( heartbeat_time >= 1000 )
		{
			std::cout << "+" << std::endl;
			udp_conn_.QueuePacket( new HeartbeatPacket( player.ID ) );
			heartbeat_time -= 1000;
		}

		// Get packets from the network
		Read();

		if( PollPacket( recvd ) )
		{
			if( ((SyncPacket*)recvd.get())->GetMode() == SYNC_RETURN )
			{
				// send the packet back to the server immidiately
				UDPSend( recvd.release( ) );
			}
			else if( ((SyncPacket*)recvd.get())->GetMode() == SYNC_SET )
			{
				globalTime = ((SyncPacket*)recvd.get( ))->GetTime( );
				break;
			}
		}

		heartbeat_time += SDL_GetTicks( ) - current_time;
		current_time = SDL_GetTicks( );
	}
	std::cout << "Server set golbal time to: " << globalTime << "ms" << std::endl;
}

bool Connection::GetMapData( const Player& player, World& world )
{
	tcp_conn_.QueuePacket( new InfoRequestPacket( player.ID, RT_MAP_DATA ) );

	std::cout << "Requesing map from server... " << std::flush;
	Uint32 sync_start_time = SDL_GetTicks( );
	Uint32 current_time = sync_start_time;
	std::unique_ptr<BasePacket> recvd;
	while( true )
	{
		if( current_time - sync_start_time > 11000 )
		{
			std::cout << "could not download the map" << std::endl;
			return false;
		}

		// Get packets from the network
		Read();

		if( PollPacket( recvd ) )
		{
			// If its the map data, save it
			if( recvd->Type() == PT_MAP_DATA )
			{
				MapDataPacket* p = (MapDataPacket*)recvd.get();

				world.SetMap( (char*)p->Data(), p->Width(), p->Height() );
				std::cout << "downloaded successfully" << std::endl;
				return true;
			}
		}
	}

	current_time = SDL_GetTicks();
}

void Connection::TCPSend( BasePacket* packet )
{
  tcp_conn_.QueuePacket( packet );
}

void Connection::UDPSend( BasePacket* packet )
{
  udp_conn_.QueuePacket( packet );
}

void Connection::Read()
{
	// Read all the pending UDP packets
	bool udp_packet_avalible = true;
	while( udp_packet_avalible )
	{
		std::unique_ptr<BasePacket> recvd = udp_conn_.GetNextPacket( );

		// if the packet was not null, add it to the queue
		if( recvd != nullptr )
			packet_queue_.push( std::move( recvd ) );
		else
			udp_packet_avalible = false;
	}

	// Read all pending TCP packets
	bool tcp_packet_avalible = true;
	while( tcp_packet_avalible )
	{
		std::unique_ptr<BasePacket> recvd = tcp_conn_.GetNextPacket();

		// if the packet was not null, add it to the queue
		if( recvd != nullptr )
			packet_queue_.push( std::move( recvd ) );
		else
			tcp_packet_avalible = false;
	}
  
}

bool Connection::PollPacket( std::unique_ptr<BasePacket>& packet )
{
	// if there is something on the queue
	if( !packet_queue_.empty() )
	{
		// pass it back to the caller
		packet = std::move( packet_queue_.front() );
		packet_queue_.pop();

		return true;
	}

	return false;
}