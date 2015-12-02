#ifndef MOVE_PACKET_H
#define MOVE_PACKET_H

#include "vec2.h"
#include "BasePacket.h"

#define MOVE_PACKET_SIZE (1 + 1 + 8 + 8 + 4)

class MovePacket : public BasePacket
{
public:
	MovePacket() : BasePacket(PT_MOVE, MOVE_PACKET_SIZE) {}
	~MovePacket() {}

	void SetID( Uint8 ID );
	void SetPosition(Vec2 position);
	void SetVelocity(Vec2 velocity);
	void SetAngle(float degrees);

	Uint8 GetID();
	Vec2 GetPosition();
	Vec2 GetVelocity();
	float GetAngle();

private:
};

#endif