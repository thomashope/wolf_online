#include <iostream>
#include <algorithm>
#include <queue>
#include <SDL2/SDL_net.h>

#include "InstantCG.h"
#include "sprite.h"
#include "input.h"
#include "world.h"
#include "Connection.h"
#include "../shared/MovePacket.h"
#include "../shared/HeartbeatPacket.h"
#include "../shared/PlayerJoinedPacket.h"
#include "../shared/MapResponsePacket.h"
using namespace InstantCG;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MAP_WIDTH 24
#define MAP_HEGIHT 24

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1177

Player player;
std::vector<Sprite> sprites;

//std::queue<BasePacket*> packet_queue;

char mapData[MAP_WIDTH * MAP_HEGIHT] =
{
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

void init(); // initalises libraries
void quit(std::string message = "");	// quits libraries and exits the program
										// pass a string to show an error on exit

int main(int argc, char* argv[])
{
	// Make Sure floats are 4 bytes in size
	// If not packets will be ill formed
	SDL_assert(sizeof(float) == 4);

	init(); // Initailise the libraries

	Connection server;

	Uint32 time = 0;			//time of current frame
	Uint32 oldTime = 0;			//time of previous frame

	// Initialise SDL and the screen
	Screen screen( SCREEN_WIDTH, SCREEN_HEIGHT, "wolf_client" );
	Input input;						// init the input handler
	player.pos.set(22.0f, 12.0f);		// x and y start position
	//bool sendPosition = false;	//

	World world(&screen);
	world.SetMap( mapData, MAP_WIDTH, MAP_HEGIHT );

	server.Connect( player, world, SERVERIP, SERVERPORT );
	server.UDPSend( new HeartbeatPacket( player.ID ) );

	// if( !TCP_connection.Connect( player, SERVERIP, SERVERPORT ) )
	// {
	// 	quit("failed to make TCP connection");
	// }
	// TCP_connection.RequestMapData( world );
	// TCP_connection.StartSenderThread();
  // TCP_connection.QueuePacket( new MapRequestPacket() );
  //
	// if( !UDP_connection.Connect( SERVERIP, SERVERPORT ) )
	// {
	// 	quit( "failed to make UDP connection" );
	// }
	// UDP_connection.StartSenderThread();

  //UDP_connection.QueuePacket( new HeartbeatPacket( player.ID ) );

	//std::vector<Sprite*> sprites;
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
		oldTime = time;
		time =  SDL_GetTicks();
		float deltaTime = (time - oldTime) / 1000.0f;

		Vec2 oldpos = player.pos;

		input.PorcessEvents();

		player.Update( world, input, deltaTime );

		world.Render( player );

		Vec2 newpos = player.pos;

		if( player.MovedSignificantly( ) )
		{			
			server.UDPSend( player.GetMovePacket() );
		}

		server.Read();
				
		std::unique_ptr<BasePacket> recvd;
		while( server.PollPacket( recvd ) )
		{
			//recvd->Print();

			if( recvd->Type() == PT_MOVE )
			{
				MovePacket* p = (MovePacket*)recvd.get();

				sprites.back().SetPos( p->GetPosition() );
			}
			else if( recvd->Type() == PT_PLAYER_JOINED )
			{
				PlayerJoinedPacket* p = (PlayerJoinedPacket*)recvd.get();

				sprites.push_back( Sprite( p->GetPosition().x, p->GetPosition().y ) );
				sprites.back().SetTexture( screen.GetRenderer(), "../../resources/sprite_1.bmp", SDL_BLENDMODE_BLEND );
			}
			else if( recvd->Type() == PT_MAP_RESPONSE )
			{
				MapResponsePacket* p = (MapResponsePacket*)recvd.get();

				//TODO: the player shouldn't be able to start untill they have the map
				world.SetMap( (char*)p->Data(), p->Width(), p->Height() );
			}
		}

		//* Render The Sprites
		{
			// sort the sprites so the are drawn from back to front
			struct ByDistance {
				ByDistance(Vec2 to):to_(to){}
				Vec2 to_;
				bool operator() (Sprite& a, Sprite& b) {
					return (a.Distance(to_) > b.Distance(to_));
				}
			} byDistance(player.pos);

			std::sort(sprites.begin(), sprites.end(), byDistance);

			// render them in the new order
			for (auto& sprite : sprites)
			{
				sprite.Render(player.pos, player.dir, player.plane, screen.GetDepthBuffer());
			}
		} //*/

		screen.Display();

	} // END OF GAME LOOP

	// cleanup
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}

void init()
{
	// initalise SDL
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << "SDL_Init: %s\n" << SDL_GetError( ) << std::endl;
		exit( 1 );
	}

	// initalise SDLNet
	if( SDLNet_Init() == -1 )
	{
		std::cout << "SDLNet_Init: %s\n" << SDLNet_GetError( ) << std::endl;
		exit( 2 );
	}
}

void quit(std::string message)
{
	if( !message.empty() )
	{
#ifdef _DEBUG
		std::cerr << "Error: " << message << std::endl;
		abort();
#else
		exit( 1 );
#endif
	}

	SDLNet_Quit();
	SDL_Quit();

	exit( 0 );
}
