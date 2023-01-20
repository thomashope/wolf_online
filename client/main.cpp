#include <iostream>
#include <algorithm>
#include <queue>
#include <SDL_net.h>

#include "Enemy.h"
#include "input.h"
#include "world.h"
#include "Connection.h"
#include "../shared/UniversalPacket.h"
#undef main

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MAP_WIDTH 24
#define MAP_HEGIHT 24

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1177

Player player;
std::vector< std::unique_ptr<Enemy> > enemies;

Uint32 localTime = 0;			// time of current frame
Uint32 oldLocalTime = 0;		// time of previous frame
Uint32 globalTime = 0;			// time syncronsied with the server

bool get_enemy( Uint8 ID, int& index );					// searches for an enemy with ID, returns true if found and sets index
void new_enemy( Screen& screen, Vec2 pos, Uint8 ID );	// adds a new enemy to the game
void delete_enemy( Uint8 ID );							// removes an enemy form the game
void enemies_render( Screen& screen );					// draws all the current enemies in the world
void enemies_update( Uint32 time );						// calls update on all the ememies
void init();											// initalises libraries
void quit(std::string message = "");					// quits libraries and exits the program
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

	server.Connect( player, world, SERVERIP, SERVERPORT, globalTime );

	std::cout << "Starting Game Loop..." << std::endl;
	while( !input.AskedToQuit() && server.Good() )	// START OF GAME LOOP
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

				// cast the recvd basePacket unique_ptr to movePacket and transfer ownership to the relevant enemy
				MovePacket* mp = (MovePacket*)recvd.get();
				int enemy = 0;
				if( get_enemy( mp->GetID(), enemy ) )
				{
					// flash a red square
					SDL_Rect rect{ 0, 0, 16, 16 };
					SDL_SetRenderDrawColor( screen.GetRenderer(), 255, 0, 0, 255 );
					SDL_RenderFillRect( screen.GetRenderer(), &rect );

					enemies[enemy]->StoreMovePacket( std::unique_ptr<MovePacket>( (MovePacket*)recvd.release() ), globalTime );
				}
			}
			else if( recvd->Type() == PT_PLAYER_JOINED )
			{
				// Add a new player
				PlayerJoinedPacket* p = (PlayerJoinedPacket*)recvd.get();
				p->Print();
				new_enemy( screen, p->GetPosition(), p->GetID() );
			}
			else if( recvd->Type( ) == PT_PLAYER_DISCONNECTED )
			{
				// Remove a player
				PlayerDisconnectedPacket* p = (PlayerDisconnectedPacket*)recvd.get( );
				p->Print();
				delete_enemy( p->GetID() );
			}
		}

		enemies_update( globalTime );
		enemies_render( screen );

		screen.Display();

	} // END OF GAME LOOP

	// cleanup
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}

void new_enemy( Screen& screen, Vec2 pos, Uint8 ID )
{
	#ifdef _WIN32
		std::string path("../../resources/sprite_1.bmp");
	#else
		std::string path("../resources/sprite_1.bmp");
	#endif

	enemies.push_back( std::unique_ptr<Enemy>(new Enemy( pos )) );
	enemies.back()->SetTexture( screen.GetRenderer(), path );
	enemies.back()->SetID( ID );
}

void delete_enemy( Uint8 ID )
{
	for( auto it = enemies.begin(); it != enemies.end(); it++ )
	{
		if( (*it)->GetID() == ID )
		{
			enemies.erase( it );
			break;
		}
	}
}

bool get_enemy( Uint8 ID, int& index )
{
	for( size_t i = 0; i < enemies.size(); i++ )
	{
		if( ID == enemies[i]->GetID() )
		{
			index = i;
			return true;
		}
	}
	return false;
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
		bool operator() ( std::unique_ptr<Enemy>& a, std::unique_ptr<Enemy>& b ) {
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
