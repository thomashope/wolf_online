#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2\SDL.h>
#include "vec2.h"

// Foward Declarations
class World;
class Input;

class Player {
public:
	Player();
	~Player() {}

	void Update( const World& world, const Input& input, float deltaTime );

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