#ifndef PLAYER_H
#define PLAYER_H

#include "vec2.h"

class Player {
public:
	Player() :pos( 2.0f, 2.0f ), dir( -1.0f, 0.0f ), plane( 0.0f, 0.66f ) {}
	~Player() {}

	Vec2 pos;		//x and y start position
	Vec2 dir;		//initial direction vector
	Vec2 plane;		//the 2d raycaster version of camera plane
};

#endif