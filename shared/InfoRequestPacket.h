#ifndef INFO_REQUEST_PACKET_H
#define INFO_REQUEST_PACKET_H

#include "BasePacket.h"

#define INFOREQUST_PACKET_SIZE (1 + 1 + 1)

enum RequestType : Uint8 {
	RT_MAP_DATA = 0,
	RT_PLAYER_LIST = 1
};

class InfoRequestPacket : public BasePacket {
public:
	InfoRequestPacket( ) : BasePacket( PT_INFO_REQUEST, INFOREQUST_PACKET_SIZE ) {}
	InfoRequestPacket( Uint8 ID, RequestType type ) : InfoRequestPacket() { SetID( ID ); SetRequested( type ); }
	~InfoRequestPacket() {}

	void SetID( Uint8 ID ) { data_[1] = ID; }
	void SetRequested( RequestType type ) { data_[2] = type; }

	Uint8 GetID() const { return data_[1]; }
	RequestType GetRequested() const { return (RequestType)data_[2]; }

	void Print() const override {
		std::cout << "Player ID: " << (int)GetID() << " asked to be sent the " << ((GetRequested() == RT_MAP_DATA) ? "map":"player list") << std::endl;
	}
};

#endif
