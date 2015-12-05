#ifndef MAP_REQUEST_PACKET_H
#define MAP_REQUEST_PACKET_H

#include <iostream>
#include "BasePacket.h"

#define MAPREQUST_PACKET_SIZE (1 + 1)

class MapRequestPacket : public BasePacket {
public:
	MapRequestPacket() : BasePacket( PT_MAP_REQUEST, MAPREQUST_PACKET_SIZE ) {}
	~MapRequestPacket() {}

	void SetID( Uint8 ID ) { data_[1] = ID; }

	Uint8 GetID( ) const { return data_[1]; }

	void Print() const override {
		std::cout << "Player ID: " << (int)GetID() << " asked to be sent the map" << std::endl;
	}
};

#endif
