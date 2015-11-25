//
// This is a port of the raycaster Lode into SDL2
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

#include <iostream>
#include <algorithm>
#include <SDL2/SDL_net.h>

#include "InstantCG.h"
#include "sprite.h"
#include "input.h"
#include "world.h"
using namespace InstantCG;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MAP_WIDTH 24
#define MAP_HEGIHT 24

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1177

// function prototypes
void print(float num);
bool processEvents();
//std::string getProjectPath(const std::string &subDir = "");
//SDL_Texture* loadImage(std::string path, bool transparent = false);

bool fullscreen = false;
//std::string projectPath = getProjectPath();

// reinterperate the bits of a float as a Uint32
// Assumes sizeof(float) == 4 !!!!
// http://stackoverflow.com/questions/20762952/most-efficient-standard-compliant-way-of-reinterpreting-int-as-float
Uint32 floatToUint32(float f)
{
	Uint32 i = 0;
	char* iPtr = (char*)&i;
	char* fPtr = (char*)&f;
	memcpy(iPtr, fPtr, 4);
	return i;
}

// globals
int mouseXDist; // horizontal distance traveled by the mouse


char mapData[MAP_WIDTH * MAP_HEGIHT] =
{
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,2,2,0,0,0,0,0,0,0,0,0,3,0,3,0,3,0,0,0,1,
  1,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,3,0,0,0,3,0,0,0,1,
  1,3,3,3,0,0,3,3,3,3,0,0,0,0,0,2,0,0,0,2,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,3,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,4,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

WallDepthInfo ZBuffer[SCREEN_WIDTH];

int main(int argc, char* argv[])
{
	SDL_Init( SDL_INIT_EVERYTHING );
	
	// MAKE SURE FLOATS ARE 4 BYTES IN SIZE, OTHERWISE PACKETS WILL BE ILL FORMED!
	SDL_assert(sizeof(float) == 4);

	//// SETUP SDL_net
	if( SDLNet_Init() == -1 ) {
		std::cout << "SDLNet_Init: %s\n" << SDLNet_GetError() << std::endl;
		exit(2);
	}
	IPaddress address;
	UDPsocket UDP_server;
	UDPpacket* UDP_send_packet;

	// Resolve server name
	if( SDLNet_ResolveHost(&address, SERVERIP, SERVERPORT) == -1 )
	{
		fprintf(stderr, "SDLNet_ResolveHost(%s %d): %s\n", SERVERIP, SERVERPORT, SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	// Open a socket on random port
	if( !(UDP_server = SDLNet_UDP_Open(0)) )
	{
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	// Allocate memory for the packet
	if( !(UDP_send_packet = SDLNet_AllocPacket(128)) )
	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	// Setup the packet
	UDP_send_packet->address.host = address.host;	// Set the destination host
	UDP_send_packet->address.port = address.port;	// Set the destination port
	UDP_send_packet->len = 10;						// Packet size in bytes

	UDP_send_packet->data[0] = 0;										// Packet Type
	UDP_send_packet->data[1] = 10;										// OwnerID
	SDLNet_Write32(floatToUint32(1.2345f), &UDP_send_packet->data[2]);	// X position
	SDLNet_Write32(floatToUint32(9.8765f), &UDP_send_packet->data[6]);	// Y position

	std::cout << "Sending hello" << std::endl;
	SDLNet_UDP_Send(UDP_server, -1, UDP_send_packet);


	// Initialise SDL and the screen
	Screen screen( SCREEN_WIDTH, SCREEN_HEIGHT, "wolf_client" );
	Input input;						// init the input handler
	Player player;						// stores player data
	player.pos.set(22.0f, 12.0f);		// x and y start position
	//player.Dir(-1.0f, 0.0f);			// initial direction vector
	//player.Plane(0.0f, 0.66f);		// the 2d raycaster version of camera plane
	bool sendPosition = false;	//

	World world(&screen);
	world.SetMap( mapData, MAP_WIDTH, MAP_HEGIHT );

	double time = 0;			//time of current frame
	double oldTime = 0;			//time of previous frame
	
	//// loading in the textures
	//Texture wall1(ren, projectPath + "../img/wall_1.bmp");
	//Texture wall2(ren, projectPath + "../img/wall_2.bmp");
	//Texture wall3(ren, projectPath + "../img/wall_3.bmp");
	//Texture wall4(ren, projectPath + "../img/wall_4.bmp");
	//Texture lights(ren, projectPath + "../img/lights_1.bmp");

	std::vector<Sprite*> sprites;
	//sprites.push_back(new Sprite(16, 16));
	//sprites.push_back(new Sprite(20, 16));
	//sprites.push_back(new Sprite(16, 20));
	//sprites[0]->SetTexture(ren, "../img/sprite_1.bmp", SDL_BLENDMODE_BLEND);
	//sprites[1]->SetTexture(ren, "../img/sprite_2.bmp", SDL_BLENDMODE_BLEND);
	//sprites[2]->SetTexture(ren, "../img/sprite_3.bmp", SDL_BLENDMODE_BLEND);
	//sprites[2]->SetScale( 0.5f, 0.5f );

	std::cout << "Starting Game Loop..." << std::endl;
	while( !input.AskedToQuit() )	// START OF GAME LOOP
	{
		input.PorcessEvents();

		world.Render( player );

		/*
		for( int x = 0; x < w; x++ )
		{
			//calculate ray position and direction
			float cameraX = 2 * x / float(w) - 1; //x-coordinate in camera space
			Vec2 rayPos(pos);
			Vec2 rayDir = dir + plane * cameraX;

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
				if( worldMap[mapX][mapY] > 0 ) hit = 1;
			}

			//Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
			if( side == 0 )
				ZBuffer[x].perpDist = fabs( (mapX - rayPos.x + (1 - stepX) / 2) / rayDir.x );
			else
				ZBuffer[x].perpDist = fabs( (mapY - rayPos.y + (1 - stepY) / 2) / rayDir.y );

			//Calculate height of line to draw on screen
			int lineHeight = abs( int( h / ZBuffer[x].perpDist ) );
			
			//choose a texture
			Texture* tex = nullptr;
			switch( worldMap[mapX][mapY] )
			{
			case 1:  tex = &wall1; break;
			case 2:  tex = &wall2; break;
			case 3:  tex = &wall3; break;
			case 4:  tex = &wall4; break;
			case 5:  tex = &lights; break;
			default: tex = &wall1; break;
			}

			//calculate value of wallX in range 0.0 - 1.0
			float wallX; //where exactly the wall was hit
			if( side == 1 )
				wallX = rayPos.x + ((mapY - rayPos.y + (1 - stepY) / 2) / rayDir.y) * rayDir.x;
			else
				wallX = rayPos.y + ((mapX - rayPos.x + (1 - stepX) / 2) / rayDir.x) * rayDir.y;

			wallX -= floor( (wallX) );

			tex->RenderStrip(x, -lineHeight / 2 + h / 2, lineHeight / 2 + h / 2, wallX);
			
			//calculate lowest and highest pixel to fill in current stripe
			ZBuffer[x].top = -lineHeight / 2 + h / 2;
			ZBuffer[x].bottom = lineHeight / 2 + h / 2;
		}
		*/

		/*// Render The Sprites
		{
			// sort the sprites so the are drawn from back to front
			struct ByDistance {
				ByDistance(Vec2 to):to_(to){}
				Vec2 to_;
				bool operator() (Sprite* a, Sprite* b) {
					return (a->Distance(to_) > b->Distance(to_));
				}
			} byDistance(player.pos);

			std::sort(sprites.begin(), sprites.end(), byDistance);
			
			// render them in the new order
			for (auto sprite : sprites)
			{					  
				sprite->Render(player.pos, player.dir, player.plane, ZBuffer);
			}
		}*/
		
	    //timing for input and FPS counter
		oldTime = time;
		time = getTicks();
		double frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds
		//redraw();
		//cls();

        // draw ground and sky
        //drawRect(0,   0, w, h/2, RGB_Black);
        //drawRect(0, h/2, w, h  , RGB_Gray);

		//speed modifiers
		float moveSpeed = (float)frameTime * 5.0f; //the constant value is in squares/second
		float rotSpeed = (float)frameTime * 90.0f; //the constant value is in degrees/second
		if( input.XMotion( ) > 0 ) rotSpeed *= input.XMotion( ) *  0.2f;
		else if( input.XMotion( ) < 0 ) rotSpeed *= input.XMotion( ) * -0.2f;
		
		int mouseX, mouseY;
		bool mouseL, mouseR;
		getMouseState(mouseX, mouseY, mouseL, mouseR);
		/*
		if (mouseL)
		{
			sprites[0]->SetPos( pos + dir * 1.5f);
		}*/

		//strafe to the right
		if( input.KeyDown( SDL_SCANCODE_D ) )
		{
			if (world.GetGrid(int(player.pos.x + player.dir.y * moveSpeed),int(player.pos.y)) == false) player.pos.x += player.dir.y * moveSpeed;
			if (world.GetGrid(int(player.pos.x), int(player.pos.y + -player.dir.x * moveSpeed)) == false) player.pos.y += -player.dir.x * moveSpeed;
			sendPosition = true;
		}
		//strafe to the left
		if( input.KeyDown( SDL_SCANCODE_A ) )
		{
			if( world.GetGrid( int( player.pos.x + -player.dir.y * moveSpeed ), int( player.pos.y ) ) == false ) player.pos.x += -player.dir.y * moveSpeed;
			if( world.GetGrid( int( player.pos.x ), int( player.pos.y + player.dir.x * moveSpeed ) ) == false )  player.pos.y += player.dir.x * moveSpeed;
			sendPosition = true;
		}
		//move forward if no wall in front of you
		if( input.KeyDown( SDL_SCANCODE_W ) )
		{
			if( world.GetGrid( int( player.pos.x + player.dir.x * moveSpeed ), int( player.pos.y ) ) == false ) player.pos.x += player.dir.x * moveSpeed;
			if( world.GetGrid( int( player.pos.x ), int( player.pos.y + player.dir.y * moveSpeed ) ) == false ) player.pos.y += player.dir.y * moveSpeed;
			sendPosition = true;
		}
		//move backwards if no wall behind you
		if( input.KeyDown( SDL_SCANCODE_S ) )
		{
			if( world.GetGrid(int( player.pos.x - player.dir.x * moveSpeed ), int( player.pos.y )) == false ) player.pos.x -= player.dir.x * moveSpeed;
			if( world.GetGrid(int( player.pos.x ), int( player.pos.y - player.dir.y * moveSpeed )) == false ) player.pos.y -= player.dir.y * moveSpeed;
			sendPosition = true;
		}
		//rotate to the right
		if( input.XMotion() > 0 )
		{
			player.dir = player.dir.rotate( -rotSpeed );
			player.plane = player.plane.rotate( -rotSpeed );
		}
		//rotate to the left
		if( input.XMotion() < 0 )
		{
			player.dir = player.dir.rotate( rotSpeed );
			player.plane = player.plane.rotate( rotSpeed );
		}

		if( sendPosition )
		{
			SDLNet_Write32( floatToUint32( player.pos.x ), &UDP_send_packet->data[2] );	// X position
			SDLNet_Write32( floatToUint32( player.pos.y ), &UDP_send_packet->data[6] );	// Y position
			SDLNet_UDP_Send( UDP_server, -1, UDP_send_packet );
			sendPosition = false;
		}

		screen.Display();

	} // END OF GAME LOOP


	// clean up
	for (auto sprite : sprites) {
		delete sprite;
	}

	SDLNet_FreePacket(UDP_send_packet);
	SDLNet_Quit( );

	screen.~Screen();

	SDL_Quit( );

	return 0;
}

/*
bool processEvents()
{
    SDL_Event event;
    mouseXDist = 0;

    readKeys();
    while( SDL_PollEvent(&event) )
    {
        if( event.type == SDL_QUIT )    return true;
        if( keyDown(SDLK_ESCAPE) )      return true;
        if( event.type == SDL_MOUSEMOTION )
        {
            mouseXDist += event.motion.xrel;
        }
    }
    return false;
}*/

/*
std::string getProjectPath(const std::string &subDir) {
	//We need to choose the path separator properly based on which
	//platform we're running on, since Windows uses a different
	//separator than most systems
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif
	//This will hold the base resource path: Lessons/res/
	//We give it static lifetime so that we'll only need to call
	//SDL_GetBasePath once to get the executable path
	static std::string baseRes;
	if (baseRes.empty()){
		//SDL_GetBasePath will return NULL if something went wrong in retrieving the path
		char *basePath = SDL_GetBasePath();
		if (basePath){
			baseRes = basePath;
			SDL_free(basePath);
		}
		else {
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
	}
	//If we want a specific subdirectory path in the resource directory
	//append it to the base path. This would be something like Lessons/res/Lesson0
	return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}*/

/*
SDL_Texture* loadImage(std::string path, bool transparent)
{
	static std::string projectPath = getProjectPath();

	// load the file into a surface
	SDL_Surface *bmp = SDL_LoadBMP(path.c_str());
	if (bmp == nullptr) {
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
	}
	else { std::cout << "Found: " << path << std::endl; }

	if (transparent) {
		// Set magenta (super pink) as the transparent colour
		int error = SDL_SetColorKey(bmp, SDL_TRUE, SDL_MapRGB(bmp->format, 255, 0, 255));
		if (error < 0) {
			std::cout << "SDL_SetColorKey Error: " << SDL_GetError() << std::endl;
		}
	}
	
	SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, bmp);	
	if (tex == nullptr){
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
	}

	if (transparent) {
		// Make the created texture actually use transparency
		int error = SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
		if (error < 0) {
			std::cout << "SDL_SetTextureBlendMode Error: " << SDL_GetError() << std::endl;
		}
	}

	SDL_FreeSurface(bmp);
    return tex;
}*/

