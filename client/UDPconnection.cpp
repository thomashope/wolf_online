#include "UDPconnection.h"
#include "../shared/BasePacket.h"
#include "../shared/MovePacket.h"

UDPConnection::UDPConnection() :
sender_thread_(nullptr)
{
	close_thread_ = false;
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

	// connected successfully
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
			SDLNet_UDP_Send( socket_, -1, &SDLpacket );

			// clean dangling ptr
			SDLpacket.data = nullptr;
			delete packet;
		}
	}

	std::cout << "UDP thread closed" << std::endl;
}

void UDPConnection::Read()
{
	UDPpacket SDLpacket;
	// join the SDLpackt to the universal packet
	SDLpacket.data = packet_.Data();
	SDLpacket.maxlen = packet_.Size();

	while( SDLNet_UDP_Recv( socket_, &SDLpacket ) )
	{
		auto recvd = packet_.CreateFromContents( );

		if( recvd )
		{
			if( recvd->Type( ) == PT_MOVE )
			{
				recvd->Print();
			}
			else if( recvd->Type() == PT_PLAYER_JOINED )
			{
				recvd->Print();
			}
			else
			{
				std::cout << "UDP type not recognised" << std::endl;
			}
		}
		else
		{
			std::cout << "UDP packet not supported" << std::endl;
		}
	}
}
