#include "world.h"

World::World( Screen& screen, int width, int height) :
width_( width ),
height_( height ),
screen_( &screen )
{
	std::string projectPath;
	//SDL_GetBasePath will return NULL if something went wrong in retrieving the path
	char *basePath = SDL_GetBasePath( );
	if( basePath ) {
		projectPath = basePath;
		SDL_free( basePath );
	} else {
		std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
	}

	// Create a default empty map
	map_.clear();
	for( int x = 0; x < width_; x++ )
	for( int y = 0; y < height_; y++ )
	{
		if( x == 0 || x == width_ - 1 || y == 0 || y == height_ - 1 )
			map_.push_back( 1 );
		else
			map_.push_back( 0 );
	}

#if defined(_WIN32)
	std::string resPath( "../../resources/" );
#else
	std::string resPath( "./resources/");
#endif

	// Load the textures
	textures_.push_back( new Texture( screen_->GetRenderer(), projectPath + resPath + "wall_1.bmp" ) );
	textures_.push_back( new Texture( screen_->GetRenderer(), projectPath + resPath + "wall_2.bmp" ) );
	textures_.push_back( new Texture( screen_->GetRenderer(), projectPath + resPath + "wall_3.bmp" ) );
	textures_.push_back( new Texture( screen_->GetRenderer(), projectPath + resPath + "wall_4.bmp" ) );
	textures_.push_back( new Texture( screen_->GetRenderer(), projectPath + resPath + "lights_1.bmp" ) );
}

World::~World()
{
	// cleanup the texturse
	for( size_t i = 0; i < textures_.size(); i++ )
	{
		delete textures_[i];
		textures_[i] = nullptr;
	}
}

void World::SetMap( char* map, int width, int height )
{
	// exit immidiately if no data was passed
	if( map == nullptr ) return;

	// store the width and height
	width_ = width;
	height_ = height;

	// throw away any data already in the vector
	map_.clear();

	// ensure the vector has enough space for the map
	map_.resize( width_ * height_, 0 );

	// fill the map from the ptr
	for( int i = 0; i < width_ * height_; i++ )
	{
		map_[i] = map[i];
	}
}

char World::GetGrid(int x, int y) const
{
	int clipped = y * height_ + x;
	if( clipped < 0 ) {
		clipped = 0;
	} else if ( clipped > width_ * height_ ) {
		clipped = width_ * height_;
	}

	return map_[clipped];
}

void World::Render(const Player& player)
{
	// This is a port of the raycaster by Lode into SDL2, significantly modified
	// http://lodev.org/cgtutor/raycasting.html
	//
	// Copyright (c) 2004-2007, Lode Vandevenne
	//
	// All rights reserved.
	//
	// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
	//
	//     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	//     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	//
	// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	for( int x = 0; x < screen_->Width(); x++ )
	{
		//calculate ray position and direction
		float cameraX = 2 * x / float( screen_->Width( ) ) - 1; //x-coordinate in camera space
		Vec2 rayPos = player.pos;
		Vec2 rayDir = player.dir + player.plane * cameraX;

		//which box of the map we're in
		int mapX = int( rayPos.x );
		int mapY = int( rayPos.y );

		//length of ray from current position to next x or y-side
		Vec2 sideDist;

		//length of ray from one x or y-side to next x or y-side
		Vec2 deltaDist;
		deltaDist.x = sqrt( 1 + (rayDir.y * rayDir.y) / (rayDir.x * rayDir.x) );
		deltaDist.y = sqrt( 1 + (rayDir.x * rayDir.x) / (rayDir.y * rayDir.y) );

		//what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;

		int hit = 0; //was there a wall hit?
		int side; //was a NS or a EW wall hit?

		//calculate step and initial sideDist
		if( rayDir.x < 0 )
		{
			stepX = -1;
			sideDist.x = (rayPos.x - mapX) * deltaDist.x;
		}
		else
		{
			stepX = 1;
			sideDist.x = (mapX + 1.0f - rayPos.x) * deltaDist.x;
		}
		if( rayDir.y < 0 )
		{
			stepY = -1;
			sideDist.y = (rayPos.y - mapY) * deltaDist.y;
		}
		else
		{
			stepY = 1;
			sideDist.y = (mapY + 1.0f - rayPos.y) * deltaDist.y;
		}

		//perform DDA
		while( hit == 0 )
		{
			//jump to next map square, OR in x-direction, OR in y-direction
			if( sideDist.x < sideDist.y )
			{
				sideDist.x += deltaDist.x;
				mapX += stepX;
				side = 0;
			}
			else
			{
				sideDist.y += deltaDist.y;
				mapY += stepY;
				side = 1;
			}
			//Check if ray has hit a wall
			if( GetGrid(mapX, mapY) > 0 ) hit = 1;
		}

		//Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
		if( side == 0 )
			screen_->GetDepthBuffer()[x].perpDist = fabs( (mapX - rayPos.x + (1 - stepX) / 2) / rayDir.x );
		else
			screen_->GetDepthBuffer()[x].perpDist = fabs( (mapY - rayPos.y + (1 - stepY) / 2) / rayDir.y );

		//Calculate height of line to draw on screen
		int lineHeight = abs( int( screen_->Height() / screen_->GetDepthBuffer()[x].perpDist ) );

		int textureIndex = GetGrid( mapX, mapY );
		if( textureIndex >= (int)textures_.size() )
		{
			textureIndex = 0;
		}

		//calculate value of wallX in range 0.0 - 1.0
		float wallX; //where exactly the wall was hit
		if( side == 1 )
			wallX = rayPos.x + ((mapY - rayPos.y + (1 - stepY) / 2) / rayDir.y) * rayDir.x;
		else
			wallX = rayPos.y + ((mapX - rayPos.x + (1 - stepX) / 2) / rayDir.x) * rayDir.y;

		wallX -= floor( (wallX) );

		textures_[textureIndex]->RenderStrip( x, -lineHeight / 2 + screen_->Height( ) / 2, lineHeight / 2 + screen_->Height( ) / 2, wallX );

		//calculate lowest and highest pixel to fill in current stripe
		screen_->GetDepthBuffer()[x].top = -lineHeight / 2 + screen_->Height() / 2;
		screen_->GetDepthBuffer()[x].bottom = lineHeight / 2 + screen_->Height() / 2;
	}
}
