#ifndef JOIN_RESPONSE_PACKET
#define JOIN_RESPONSE_PACKET

#include "BasePacket.h"

enum JoinResponse {
	JR_OK = 0,
	JR_REJECT= 1
};

// 0, type
// 1, response
// 2, givenID

class JoinResponsePacket : public BasePacket {
public:
	JoinResponsePacket() :BasePacket( 3 ) {
		data_[0] = PT_JOIN_RESPONSE;
	}
	~JoinResponsePacket() {}

	void SetResponse( JoinResponse jr ) { data_[1] = jr; }
	void SetGivenID( Uint8 ID ) { data_[2] = ID; }

	JoinResponse GetResponse( ) const { return (JoinResponse)data_[1]; }
	Uint8 GetGivenID( ) const { return (Uint8)data_[2]; }
};

#endif