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

class World;
class Player;
class BasePacket;

class TCPConnection {
public:
	TCPConnection();
	~TCPConnection();

	// returns true if a connection was established, or false if failed to connect
	bool Connect(Player& player, std::string host, Uint16 port);

	// returns true if the server sent the map successfully, or false if there was an error
	bool RequestMapData( World& world );

	// adds a packet to the queue
	void QueuePacket( BasePacket* packet );

	// creates a thread that sends packets as soon as they are added to the queue
	void StartSenderThread();

private:
	IPaddress address_;	// Address of the recipient
	TCPsocket socket_;	// socket to send over

	// TODO: remove this
	// data received is stored in the bufferr
	Uint8* buffer_;
	const int MAX_BUFFER{ 256 };

	std::atomic_bool close_thread_;
	std::mutex queue_mtx_;
	std::queue< std::unique_ptr<BasePacket> > packet_queue_;
	std::thread* sender_thread_;

	// attach to the sender thread
	void SendPackets();
};

#endif