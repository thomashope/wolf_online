#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

#include <SDL2/SDL_net.h>
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include "../shared/UniversalPacket.h"

class BasePacket;

class UDPConnection {
public:
	UDPConnection();
	~UDPConnection();

	bool Connect( std::string host, Uint16 port );

	// adds a packet to the queue
	void QueuePacket( BasePacket* packet );

	// Create a thread that sends packets as soon as they are added to the queue
	void StartSenderThread();

	// Pull the next packet form the network
	std::unique_ptr<BasePacket> GetNextPacket( );

	// Send the single next packet in the queue
	void SendPacket();

private:
	IPaddress address_;
	UDPsocket socket_;
	UniversalPacket packet_;	// Data is received into this packet, TODO: can I remove this?

	std::atomic_bool close_thread_;
	std::mutex queue_mtx_;
	std::queue< std::unique_ptr<BasePacket> > packet_queue_;
	std::thread* sender_thread_;

	// Sends queued packets, attached to the sender thread
	void SendPackets();
};

#endif
