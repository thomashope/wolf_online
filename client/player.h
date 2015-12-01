#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2\SDL.h>
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
	bool SignificantMove();
	
	// creates a packet with the players current data
	// TODO: use unique_ptr instead of raw ptr
	BasePacket* GetMovePacket();

	Vec2 pos;		// x and y start position
	Vec2 dir;		// initial direction vector
	Vec2 plane;		// the 2d raycaster version of a camera plane

	Uint8 ID;		// Used to distinguish between players
					// The sever will allocate an ID on conneciton
private:
	float moveSpeed_;
	float rotSpeed_;
};

#endif