#include "TCPConnection.h"
#include "world.h"
#include "player.h"
#include "../shared/UniversalPacket.h"

TCPConnection::TCPConnection() :
sender_thread_( nullptr )
{
	// allocate the buffer
	buffer_ = new Uint8[MAX_BUFFER];

	close_thread_ = false;

	socket_set_ = SDLNet_AllocSocketSet( 1 );
}

TCPConnection::~TCPConnection()
{
	// clean up the thread
	close_thread_ = true;
	if( sender_thread_ )
		sender_thread_->join();

	// free the buffer
	delete[] buffer_;
	buffer_ = nullptr;

	SDLNet_FreeSocketSet( socket_set_ );
}

bool TCPConnection::Connect( Player& player, std::string host, Uint16 port )
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
	request.SetPosition( player.pos );
	
	// Ask to join the server
	SDLNet_TCP_Send( socket_, request.Data(), request.Size() );

	JoinResponsePacket response;

	// read the response
	//TODO: check return value for errors
	SDLNet_TCP_Recv( socket_, response.Data(), response.Size() );

	if( response.GetResponse() == JR_REJECT )
	{
		// exit if rejected
		std::cout << "Your connection was rejected by the server" << std::endl;
		return false;
	}

	player.ID = response.GetGivenID();
	std::cout << "The server assigned you an ID of " << (int)player.ID << std::endl;

	// Add the socket to the set
	SDLNet_TCP_AddSocket( socket_set_, socket_ );

	// return success
	return true;
}


void TCPConnection::AttachWorld( World* world )
{
	world_ = world;
}

void TCPConnection::QueuePacket( BasePacket* packet )
{
	queue_mtx_.lock();

	packet_queue_.push( std::unique_ptr<BasePacket>( packet ) );

	queue_mtx_.unlock();
}

void TCPConnection::StartSenderThread()
{
	// create a thread if one doesn't already exist
	if( sender_thread_ == nullptr )
		sender_thread_ = new std::thread( std::mem_fun( &TCPConnection::SendPackets ), this );
}

void TCPConnection::SendPackets()
{
	while( !close_thread_ )
	{
		//TODO: stopy the busy waiting

		if( !packet_queue_.empty() )
		{
			queue_mtx_.lock();

			// remove the packet form the queue
			std::unique_ptr<BasePacket> packet = std::move( packet_queue_.front() );
			packet_queue_.pop();

			queue_mtx_.unlock();

			// send the front packet
			SDLNet_TCP_Send(socket_, packet->Data(), packet->Size());
		}
	}

	std::cout << "TCP thread closed" << std::endl;
}

std::unique_ptr<BasePacket> TCPConnection::GetNextPacket()
{
	SDLNet_CheckSockets( socket_set_, 0 );

	if( SDLNet_SocketReady( socket_ ) )
	{
		//if( SDLNet_TCP_Recv( socket_, packet_.Data( ), packet_.Size( ) ) > 0 )
		// read the first byte
		if( SDLNet_TCP_Recv( socket_, packet_.Data(), 1 ) > 0 )
		{
			//*
			int bytesRemaining;
			switch( *packet_.Data() )
			{
			case PT_HEARTBEAT:
				bytesRemaining = HEARTBEAT_PACKET_SIZE - 1;
				break;
			case PT_JOIN_REQUEST:
				bytesRemaining = JOINREQUEST_PACKET_SIZE - 1;
				break;
			case PT_JOIN_RESPONSE:
				bytesRemaining = JOINRESPONSE_PACKET_SIZE - 1;
				break;
			case PT_INFO_REQUEST:
				bytesRemaining = INFOREQUST_PACKET_SIZE - 1;
				break;
			case PT_MAP_DATA:
				bytesRemaining = MAPDATA_PACKET_SIZE - 1;
				break;
			case PT_MOVE:
				bytesRemaining = MOVE_PACKET_SIZE - 1;
				break;
			case PT_PLAYER_JOINED:
				bytesRemaining = PLAYERJOINED_PACKET_SIZE - 1;
				break;
			default:
				std::cout << "TCP Unknown Packet Size!!!" << std::endl;
				// TODO: read all the packets from the queue
				// read the entire buffer
				bytesRemaining = packet_.Size() - 1;
				break;
			}
			// read the rest of the packet
			Uint8* packet_contents = packet_.Data();
			packet_contents++;
			if( SDLNet_TCP_Recv( socket_, packet_contents, bytesRemaining ) < 0 )
			{
				std::cout << "TCP could not get packet contents" << std::endl;
			}
			//*/

			std::unique_ptr<BasePacket> recvd = packet_.CreateFromContents();
			if( recvd )
			{
				return recvd;
			} else {
				std::cout << "TCP Packet type not recognised" << std::endl;
				return nullptr;
			}
		}
		else
		{
			std::cout << "SDLNet_TCP_Recv: " << SDLNet_GetError() << std::endl;
			//TODO: tcp error occured, do something, quit?
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

/*
void TCPConnection::Read()
{
	SDLNet_CheckSockets( socket_set_, 0 );

	while( SDLNet_SocketReady( socket_ ) )
	{
		// Receive data directly into the universal packet
		//TODO: check a complete packet is being received
		if( SDLNet_TCP_Recv( socket_, packet_.Data(), packet_.Size() ) > 0 )
		{
			auto recvd = packet_.CreateFromContents();

			// check if its a join request
			if( recvd )
			{
				if( recvd->Type() == PT_HEARTBEAT )
				{
					HeartbeatPacket* packet = (HeartbeatPacket*)recvd.get();

					std::cout << "ID: " << (int)packet->GetID() << " sent heartbeat" << std::endl;
				}
				else if( recvd->Type() == PT_MOVE )
				{
					MovePacket* packet = (MovePacket*)recvd.get();

					std::cout << "ID: " << (int)packet->GetID() << " x: " << packet->GetPosition( ).x << " y: " << packet->GetPosition( ).y << std::endl;
				}
				else if( recvd->Type() == PT_MAP_RESPONSE )
				{
					MapResponsePacket* packet = (MapResponsePacket*)recvd.get();

					std::cout << "server sent map data " << std::endl;
					world_->SetMap( packet->GetMapData(), packet->Width(), packet->Height() );
				}
				else if( recvd->Type() == PT_PLAYER_JOINED )
				{
					PlayerJoinedPacket* packet = (PlayerJoinedPacket*)recvd.get();

					std::cout << "New player: " << (int)packet->GetID() << std::endl;
				}
				else
				{
					std::cout << "TCP type not recognised: " << (int)recvd->Type() << std::endl;
				}
			}
			else
			{
				std::cout << "TCP packet not supported" << std::endl;
			}
		}
		else
		{
			std::cout << "SDLNet_TCP_Recv: " << SDLNet_GetError() << std::endl;
			//TODO: tcp error occured, do something, quit?
		}

		SDLNet_CheckSockets( socket_set_, 0 );
	}
}
*/