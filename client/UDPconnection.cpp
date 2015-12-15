#include "UDPconnection.h"
#include "../shared/BasePacket.h"
#include "../shared/MovePacket.h"

UDPConnection::UDPConnection() :
sender_thread_(nullptr)
{
	close_thread_ = false;

	connection_good_ = false;
}

UDPConnection::~UDPConnection()
{
	// clean up the thread
	close_thread_ = true;
	if (sender_thread_)
		sender_thread_->join();
}

bool UDPConnection::Connect( std::string host, Uint16 port )
{
	// Resolve server name
	if( SDLNet_ResolveHost( &address_, host.c_str(), port ) == -1 )
	{
		fprintf( stderr, "SDLNet_ResolveHost(%s %d): %s\n", host.c_str( ), port, SDLNet_GetError( ) );
		return false;
	}
	// Open a socket on random port
	if( !(socket_ = SDLNet_UDP_Open( 0 )) )
	{
		fprintf( stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError( ) );
		return false;
	}

	// connected successfully
	connection_good_ = true;
	return true;
}

void UDPConnection::QueuePacket( BasePacket* packet )
{
	queue_mtx_.lock();

	packet_queue_.push( std::unique_ptr<BasePacket>( packet ) );

	queue_mtx_.unlock();
}

void UDPConnection::StartSenderThread()
{
	// create a thread if one doesn't already exist
	if (sender_thread_ == nullptr)
		sender_thread_ = new std::thread(std::mem_fun(&UDPConnection::SendPackets), this);
}

void UDPConnection::SendPacket()
{
	if( !packet_queue_.empty() )
	{
		// Create a SDLNet packet to fill with data
		UDPpacket SDLpacket;

		// setup the SDLpackets destination
		SDLpacket.address.host = address_.host;
		SDLpacket.address.port = address_.port;

		// protect the queue
		queue_mtx_.lock();

		// remove the packet from the front of the queue
		std::unique_ptr<BasePacket> packet = std::move( packet_queue_.front() );
		packet_queue_.pop();

		queue_mtx_.unlock();

		// point the SDL packet to the data in the BasePacket
		SDLpacket.data = packet->Data();
		SDLpacket.len = packet->Size();
		SDLpacket.maxlen = packet->Size();

		// Send the packet
		if( SDLNet_UDP_Send( socket_, -1, &SDLpacket ) <= 0 )
		{
			// Some error occured
			connection_good_ = false;
		}
	}
}

void UDPConnection::SendPackets()
{
	// Create a SDLNet packet to fill with data
	UDPpacket SDLpacket;

	// setup the SDLpackets destination
	SDLpacket.address.host = address_.host;
	SDLpacket.address.port = address_.port;

	while( !close_thread_ )
	{
		if( !packet_queue_.empty() )
		{
			// protect the queue
			queue_mtx_.lock();

			// remove the packet from the front of the queue
			BasePacket* packet = packet_queue_.front().release();
			packet_queue_.pop();

			queue_mtx_.unlock();

			// point the SDL packet to the data in the BasePacket
			SDLpacket.data = packet->Data();
			SDLpacket.len = packet->Size();
			SDLpacket.maxlen = packet->Size();

			// Send the packet
			if( SDLNet_UDP_Send( socket_, -1, &SDLpacket ) <= 0 )
			{
				connection_good_ = false;
			}

			// clean dangling ptr
			SDLpacket.data = nullptr;
			delete packet;
		}
	}

	std::cout << "UDP thread closed" << std::endl;
}

std::unique_ptr<BasePacket> UDPConnection::GetNextPacket()
{
	UDPpacket SDLpacket;
	// join the SDLpackt to the universal packet
	SDLpacket.data = packet_.Data( );
	SDLpacket.maxlen = packet_.Size( );

	if( SDLNet_UDP_Recv( socket_, &SDLpacket ) )
	{
		return packet_.CreateFromContents();
	}
	else
	{
		return nullptr;
	}
}