#include <iostream>
#include <algorithm>
#include <queue>
#include <SDL2/SDL_net.h>

#include "Enemy.h"
#include "input.h"
#include "world.h"
#include "Connection.h"
#include "../shared/UniversalPacket.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MAP_WIDTH 24
#define MAP_HEGIHT 24

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1177

Player player;
std::vector<Enemy*> enemies;

Uint32 localTime = 0;			// time of current frame
Uint32 oldLocalTime = 0;		// time of previous frame
Uint32 globalTime = 0;			// time syncronsied with the server

void new_enemy( Screen& screen, Vec2 pos );	// adds a new enemy to the game
void enemies_render( Screen& screen );		// draws all the current enemies in the world
void enemies_update( Uint32 time );			// calls update on all the ememies
void init();								// initalises libraries
void quit(std::string message = "");		// quits libraries and exits the program
											// pass a string to show an error on exit

int main(int argc, char* argv[])
{
	// Make Sure floats are 4 bytes in size
	// If not packets will be ill formed
	SDL_assert(sizeof(float) == 4);

	init(); // Initailise the libraries

	Connection server;

	// Initialise SDL and the screen
	Screen screen( SCREEN_WIDTH, SCREEN_HEIGHT, "wolf_client" );
	World world( screen, MAP_WIDTH, MAP_HEGIHT );
	Input input;						// init the input handler
	player.pos.set(22.0f, 12.0f);		// x and y start position

	server.Connect( player, world, SERVERIP, SERVERPORT );
	server.UDPSend( new HeartbeatPacket( player.ID ) );

	//enemies.push_back(new Enemy(16, 16));
	//enemies.push_back(new Enemy(20, 16));
	//enemies.push_back(new Enemy(16, 20));
	//enemies[0]->SetTexture(ren, "../img/sprite_1.bmp" );
	//enemies[1]->SetTexture(ren, "../img/sprite_2.bmp" );
	//enemies[2]->SetTexture(ren, "../img/sprite_3.bmp" );
	//enemies[2]->SetScale( 0.5f, 0.5f );

	std::cout << "Starting Game Loop..." << std::endl;
	while( !input.AskedToQuit() )	// START OF GAME LOOP
	{
		oldLocalTime = localTime;
		localTime = SDL_GetTicks();
		globalTime += localTime - oldLocalTime;
		float deltaTime = (localTime - oldLocalTime) / 1000.0f;

		input.PorcessEvents();

		player.Update( world, input, deltaTime );

		world.Render( player );

		// send a move packet only when there was some significant change to the player position
		if( player.MovedSignificantly( globalTime ) )
		{
			server.UDPSend( player.GetMovePacket( globalTime ) );
		}

		server.Read();

		std::unique_ptr<BasePacket> recvd;
		while( server.PollPacket( recvd ) )
		{
			//recvd->Print();

			if( recvd->Type() == PT_MOVE && !enemies.empty() )
			{
				// flash a red square
				SDL_Rect rect{ 0, 0, 16, 16 };
				SDL_SetRenderDrawColor( screen.GetRenderer(), 255, 0, 0, 255 );
				SDL_RenderFillRect( screen.GetRenderer(), &rect );

				// TODO: support multple clients
				// cast the recvd basePacket unique_ptr to movePacket and transfer ownership to the relevant enemy
				enemies.back()->StoreMovePacket( std::unique_ptr<MovePacket>( (MovePacket*)recvd.release() ), globalTime );
			}
			else if( recvd->Type() == PT_PLAYER_JOINED )
			{
				PlayerJoinedPacket* p = (PlayerJoinedPacket*)recvd.get();

				p->Print();

				new_enemy( screen, p->GetPosition() );
			}
			else if( recvd->Type() == PT_MAP_RESPONSE )
			{
				MapResponsePacket* p = (MapResponsePacket*)recvd.get();

				//TODO: the player shouldn't be able to start untill they have the map
				world.SetMap( (char*)p->Data(), p->Width(), p->Height() );
			}
			else if( recvd->Type() == PT_SYNC )
			{
				if( ((SyncPacket*)recvd.get())->GetMode() == SYNC_RETURN )
				{
					// send the packet back to the server immidiately
					server.UDPSend( recvd.release() );
					std::cout << "Replied to sync" << std::endl;
				}
				else
				{
					std::cout << "Server set global time" << std::endl;
					// TODO: keep asking for global time if not already set
					globalTime = ((SyncPacket*)recvd.get())->GetTime();
				}
			}
		}

		enemies_update( globalTime );
		enemies_render( screen );

		screen.Display();

	} // END OF GAME LOOP

	// cleanup
	for( auto& e : enemies ) delete e;
	enemies.clear();

	SDLNet_Quit();
	SDL_Quit();
	return 0;
}

void new_enemy( Screen& screen, Vec2 pos )
{
	#ifdef _WIN32
		std::string path("../../resources/sprite_1.bmp");
	#else
		std::string path("../resources/sprite_1.bmp");
	#endif

	enemies.push_back( new Enemy( pos ) );
	enemies.back()->SetTexture( screen.GetRenderer(), path );
}

void enemies_update( Uint32 time )
{
	for( auto& e : enemies )
	{
		e->Update( time );
	}
}

void enemies_render( Screen& screen )
{
	// sort the enemies so the are drawn from back to front
	struct ByDistance {
		ByDistance( Vec2 to ) :to_( to ){}
		Vec2 to_;
		bool operator() ( Enemy* a, Enemy* b ) {
			return (a->Distance( to_ ) > b->Distance( to_ ));
		}
	} byDistance( player.pos );

	// sort the vector so they are drawn from back to front
	std::sort( enemies.begin(), enemies.end(), byDistance );

	// render them in the new order
	for( auto& e : enemies )
	{
		e->Render( player, screen.GetDepthBuffer() );
	}
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
