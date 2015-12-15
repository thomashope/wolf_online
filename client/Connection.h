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

  bool Connect( Player& player, World& world, std::string host, Uint16 port, Uint32& globalTime );

  void TCPSend( BasePacket* packet );
  void UDPSend( BasePacket* packet );

  // Returns false if something went wrong
  bool Good();

  // Pulls all pending packets and adds them to the queue
  void Read();

  bool PollPacket( std::unique_ptr<BasePacket>& packet );

private:
	bool SyncTimeWithServer( const Player& player, Uint32& globalTime );
	bool GetMapData( const Player& player, World& world );

	TCPConnection tcp_conn_;
	UDPConnection udp_conn_;

	std::queue< std::unique_ptr<BasePacket> > packet_queue_;

	std::atomic_bool close_thread_;	// closes the sender thread when true
	std::thread* sender_thread_;	// 

	// spins off a thread to send packets
	void StartSenderThread();

	// Sends both TCP and UDP packets, attach to the sender thread
	void SendPackets();	
};

#endif
