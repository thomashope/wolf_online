#ifndef MAP_RESPONSE_PACKET_H
#define MAP_RESPONSE_PACKET_H

#include "BasePacket.h"

#define MAPDATA_PACKET_SIZE (24*24 + 1)

class MapDataPacket : public BasePacket
{
public:
	MapDataPacket( ) : BasePacket( PT_MAP_DATA, MAPDATA_PACKET_SIZE ) {}
	~MapDataPacket() {}

	// copy the 24*24 map data into the packet
	void SetMapData(char* data) {
		memcpy(&data_[1], data, 24 * 24);
	}

	char* GetMapData() const { return (char*)&data_[1]; }

	unsigned int Width() const { return 24; }
	unsigned int Height() const { return 24; }

	void Print() const override {
		std::cout << "Recvd map data of Width " << Width() << " and Height " << Height() << std::endl;
	}
};

#endif