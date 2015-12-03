#ifndef CONNECTION_H
#define CONNECTION_H

#include "TCPConnection.h"
#include "UDPConnection.h"

class Connection
{
public:
  Connection();
  ~Connection();


  bool Connect( Player& player, World& world, std::string host, Uint16 port );

  void TCPSend( BasePacket* packet );
  void UDPSend( BasePacket* packet );

  // Returns false if something went wrong
  bool Good();

  // Pulls all pending packets and adds them to the queue
  void Read();

private:
  TCPConnection tcp_conn_;
  UDPConnection udp_conn_;
  bool tcp_good_;
  bool udp_good_;
};

#endif
