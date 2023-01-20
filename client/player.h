#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
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
	bool MovedSignificantly( Uint32 globalTime );

	// creates a packet with the players current data
	BasePacket* GetMovePacket( Uint32 globalTime );

	Vec2 pos;		// x and y start position
	Vec2 dir;		// initial direction vector
	Vec2 plane;		// the 2d raycaster version of a camera plane

	Uint8 ID;		// Used to distinguish between players
					// The sever will allocate an ID on conneciton

	Vec2 GetVelocity() { return vel_; }	

private:
	Vec2 vel_;
	bool send_move_; // true when a move packet is worth sending

	float moveSpeed_;
	float rotSpeed_;

	Uint32 time_move_sent_;	// The last time the player sent a move packet
};

#endif
