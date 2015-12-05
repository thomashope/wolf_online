#ifndef CONNECTION_H
#define CONNECTION_H

#include "TCPConnection.h"
#include "UDPConnection.h"

#include <queue>

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

  bool PollPacket( std::unique_ptr<BasePacket>& packet );

private:
  TCPConnection tcp_conn_;
  UDPConnection udp_conn_;
  bool tcp_good_;
  bool udp_good_;

  std::queue< std::unique_ptr<BasePacket> > packet_queue_;
};

#endif
