#ifndef JOIN_REQUEST_PACKET_H
#define JOIN_REQUEST_PACKET_H

#include "BasePacket.h"

#define JOINREQUEST_PACKET_SIZE 6

class JoinRequestPacket : public BasePacket
{
public:
	JoinRequestPacket() :BasePacket(PT_JOIN_REQUEST, JOINREQUEST_PACKET_SIZE)
	{
		data_[1] = 'H';
		data_[2] = 'E';
		data_[3] = 'L';
		data_[4] = 'L';
		data_[5] = 'O';
	}

	~JoinRequestPacket() {}
};

#endif