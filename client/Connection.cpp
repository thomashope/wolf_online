#include "Connection.h"
#include "world.h"
#include "../shared/BasePacket.h"
#include "../shared/MapRequestPacket.h"
#include "../shared/HeartbeatPacket.h"

Connection::Connection()
{

}

Connection::~Connection()
{

}

bool Connection::Connect( Player& player, World& world, std::string host, Uint16 port )
{
  std::cout << "Creating TCP connection" << std::endl;
  if( !tcp_conn_.Connect( player, host, port ) )
    return false;

  tcp_conn_.QueuePacket( new MapRequestPacket( player.ID ) );

  if( !udp_conn_.Connect( host, port ) )
    return false;

  udp_conn_.QueuePacket( new HeartbeatPacket( player.ID ) );

  // each connection sends packets on a differnt as soon as they are avalible
  tcp_conn_.StartSenderThread();
  udp_conn_.StartSenderThread();
  return true;
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
		if( recvd )
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
		if( recvd )
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