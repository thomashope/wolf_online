#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include "screen.h"
#include "player.h"
#include "texture.h"

class World {
public:
	World(Screen* screen);
	~World();

	void SetMap( char* map, int width, int height );

	void Render(const Player& player);

	inline char GetGrid( int x, int y ) const { return map_[y * height_ + x]; }

private:

	std::vector<char> map_;
	int width_;
	int height_;

	std::vector<Texture*> textures_;
	Screen* screen_;
};

#endif