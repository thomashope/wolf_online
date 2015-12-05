#ifndef JOIN_REQUEST_PACKET_H
#define JOIN_REQUEST_PACKET_H

#include "BasePacket.h"
#include "vec2.h"

#define JOINREQUEST_PACKET_SIZE (1  + 8)

class JoinRequestPacket : public BasePacket
{
public:
	JoinRequestPacket() :BasePacket( PT_JOIN_REQUEST, JOINREQUEST_PACKET_SIZE ) {}
	~JoinRequestPacket() {}

	void SetPosition( Vec2 position );

	Vec2 GetPosition( ) const;
};

#endif