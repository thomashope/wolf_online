#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "../shared/vec2.h"

// Foward Declarations
class World;
class Input;
class BasePacket;

class Player {
public:
	Player();
	~Player() {}

	void Update( const World& world, const Input& input, float deltaTime );

	// true if the player has moved enough that it's worth telling the server
	bool MovedSignificantly();

	// creates a packet with the players current data
	// TODO: use unique_ptr instead of raw ptr, untill then caller should delete the packet
	BasePacket* GetMovePacket( Uint32 globalTime );

	Vec2 pos;		// x and y start position
	Vec2 dir;		// initial direction vector
	Vec2 plane;		// the 2d raycaster version of a camera plane

	Uint8 ID;		// Used to distinguish between players
					// The sever will allocate an ID on conneciton

	Vec2 GetVelocity() { return vel_; }	

private:
	Vec2 vel_;

	float moveSpeed_;
	float rotSpeed_;

	Uint32 timeSinceLastMove_;
};

#endif
