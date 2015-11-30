#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include "screen.h"
#include "player.h"
#include "texture.h"
#include "TCPConnection.h"

class World {
public:
	World(Screen* screen);
	~World();

	void Render(const Player& player);

	inline char GetGrid( int x, int y ) const { return map_[y * height_ + x]; }

	void SetMap( char* map, int width, int height );

private:

	std::vector<char> map_;
	int width_;
	int height_;

	std::vector<Texture*> textures_;
	Screen* screen_;
};

#endif