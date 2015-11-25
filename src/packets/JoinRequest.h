#ifndef JOIN_REQUEST_PACKET_H
#define JOIN_REQUEST_PACKET_H

#include "BasePacket.h"

class JoinRequestPacket : public BasePacket
{
public:
	JoinRequestPacket() :BasePacket( 6 ) {
		data_[0] = PT_JOIN_REQUEST;
		data_[1] = 'H';
		data_[2] = 'E';
		data_[3] = 'L';
		data_[4] = 'L';
		data_[5] = 'O';
	}
	~JoinRequestPacket( ) {
	}
};

#endif