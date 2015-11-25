#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <SDL2/SDL_net.h>
#include <string>

class World;
class Player;

class TCPConnection {
public:
	TCPConnection();
	~TCPConnection();

	// returns true if a connection was established, or false if failed to connect
	bool Connect(Player& player, std::string host, Uint16 port);

	// returns true if the server sent the map successfully, or false if there was an error
	bool RequestMapData( World& world );

	// Released any memory allocated to store the map from the server
	void FreeMapData();

private:
	IPaddress address_;
	TCPsocket socket_;

	// data received is stored in the buffer
	Uint8* buffer_;
	const int MAX_BUFFER{ 256 };

	char* mapData_;
};

#endif