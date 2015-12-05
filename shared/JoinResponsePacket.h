#ifndef JOIN_RESPONSE_PACKET_H
#define JOIN_RESPONSE_PACKET_H

#include "BasePacket.h"

#define JOINRESPONSE_PACKET_SIZE 3

enum JoinResponse {
	JR_OK = 0,
	JR_REJECT= 1
};

// 0, type
// 1, response
// 2, givenID

class JoinResponsePacket : public BasePacket {
public:
	JoinResponsePacket() : BasePacket(PT_JOIN_RESPONSE, JOINRESPONSE_PACKET_SIZE) {}
	~JoinResponsePacket() {}

	void SetResponse( JoinResponse jr ) { data_[1] = jr; }
	void SetGivenID( Uint8 ID ) { data_[2] = ID; }

	JoinResponse GetResponse() const { return (JoinResponse)data_[1]; }
	Uint8 GetGivenID() const { return (Uint8)data_[2]; }
};

#endif