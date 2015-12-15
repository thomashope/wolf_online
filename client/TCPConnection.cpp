#include "TCPConnection.h"
#include "world.h"
#include "player.h"
#include "../shared/UniversalPacket.h"

TCPConnection::TCPConnection() :
sender_thread_( nullptr )
{

	close_thread_ = false;

	connection_good_ = false;

	socket_set_ = SDLNet_AllocSocketSet( 1 );
}

TCPConnection::~TCPConnection()
{
	// clean up the thread
	close_thread_ = true;
	if( sender_thread_ )
		sender_thread_->join();

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
	if( SDLNet_TCP_Recv( socket_, response.Data(), response.Size() ) <= 0 )
	{
		std::cerr << "SDLNet_TCP_Recv error: " << SDLNet_GetError( ) << std::endl;
		return false;
	}

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
	connection_good_ = true;
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

void TCPConnection::SendPacket()
{
	if( !packet_queue_.empty( ) )
	{
		queue_mtx_.lock();

		// remove the packet form the queue
		std::unique_ptr<BasePacket> packet = std::move( packet_queue_.front( ) );
		packet_queue_.pop();

		queue_mtx_.unlock();

		// send the front packet
		if( SDLNet_TCP_Send( socket_, packet->Data( ), packet->Size( ) ) < (int)packet->Size() )
		{
			// something terrible went wrong
			connection_good_ = false;
		}
	}
}

void TCPConnection::SendPackets()
{
	while( !close_thread_ )
	{
		if( !packet_queue_.empty() )
		{
			queue_mtx_.lock();

			// remove the packet form the queue
			std::unique_ptr<BasePacket> packet = std::move( packet_queue_.front() );
			packet_queue_.pop();

			queue_mtx_.unlock();

			// send the front packet
			if( SDLNet_TCP_Send( socket_, packet->Data(), packet->Size() ) < (int)packet->Size() )
			{
				// something terrible went wrong
				connection_good_ = false;
			}
		}
	}

	std::cout << "TCP thread closed" << std::endl;
}

std::unique_ptr<BasePacket> TCPConnection::GetNextPacket()
{
	SDLNet_CheckSockets( socket_set_, 0 );

	if( SDLNet_SocketReady( socket_ ) )
	{
		// read the first byte
		if( SDLNet_TCP_Recv( socket_, packet_.Data(), 1 ) > 0 )
		{
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
			case PT_PLAYER_DISCONNECTED:
				bytesRemaining = PLAYERDISCONNECTED_PACKET_SIZE - 1;
				break;
			default:
				std::cout << "TCP Unknown Packet Size!!!" << std::endl;
				// read the entire buffer, and hope that clears it out
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
			connection_good_ = false;
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}