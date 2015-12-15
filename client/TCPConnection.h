#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include "../shared/UniversalPacket.h"

class World;
class Player;
class BasePacket;

class TCPConnection {
public:
	TCPConnection();
	~TCPConnection();

	// returns true if a connection was established, or false if failed to connect
	bool Connect( Player& player, std::string host, Uint16 port );

	// Returns false if something went wrong
	bool Good() { return connection_good_; }

	// returns true if the server sent the map successfully, or false if there was an error
	void AttachWorld( World*  world );

	// adds a packet to the queue
	void QueuePacket( BasePacket* packet );

	// creates a thread that sends packets as soon as they are added to the queue
	void StartSenderThread();

	// Receives the single next packet from the network
	std::unique_ptr<BasePacket> GetNextPacket();

	// Send the next packet in the queue
	void SendPacket();

private:
	IPaddress address_;	// Address of the recipient
	TCPsocket socket_;	// socket to send over
	SDLNet_SocketSet socket_set_;
	UniversalPacket packet_;

	// Used to update the world map
	World* world_;
	std::atomic_bool connection_good_;

	std::atomic_bool close_thread_;
	std::thread* sender_thread_;
	std::mutex queue_mtx_;
	std::queue< std::unique_ptr<BasePacket> > packet_queue_;

	// attach to the sender thread
	void SendPackets();
};

#endif
