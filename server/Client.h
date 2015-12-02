#ifndef CLIENT_H
#define CLIENT_H

#include "../shared/BasePacket.h"

class Client
{
public:
	Client();
	~Client();

	void UDPSend( BasePacket* packet );

	void TCPSend( BasePacket* packet );

private:



};

#endif