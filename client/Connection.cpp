#include "Connection.h"
#include "world.h"
#include "../shared/BasePacket.h"

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

  //std::cout << "Getting Map data" << std::endl;
  tcp_conn_.AttachWorld( &world );
  //tcp_conn.QueuePacket( new MapRequestPacket );

  if( !udp_conn_.Connect( host, port ) )
    return false;

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
  udp_conn_.Read();

  //tcp_conn_.Read();
  auto recvd = tcp_conn_.GetNextPacket();
  if( recvd )
    recvd->Print();
}
