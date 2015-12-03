#ifndef MAP_REQUEST_PACKET_H
#define MAP_REQUEST_PACKET_H

#include "BasePacket.h"

#define MAPREQUST_PACKET_SIZE 1

class MapRequestPacket : public BasePacket {
public:
	MapRequestPacket() : BasePacket(PT_MAP_REQUEST, MAPREQUST_PACKET_SIZE)
	{
	}

	~MapRequestPacket() {}

};

#endif
