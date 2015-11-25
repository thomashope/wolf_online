#include "screen.h"
#include <iostream>

Screen::Screen( int width, int height, std::string title ) :
width_(width ),
height_(height ),
renderer_(nullptr ),
window_(nullptr ),
depthBuffer_(nullptr )
{
	SDL_Init(SDL_INIT_EVERYTHING);

	// create the window
	window_ = SDL_CreateWindow( title.c_str( ),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width_, height_,
		SDL_WINDOW_SHOWN );
	// check for errors
	if( window_ == NULL )
	{
		std::cout << "Window error: " << SDL_GetError( ) << std::endl;
		SDL_Quit();
		std::exit( 1 );
	}

	// create the renderer
	renderer_ = SDL_CreateRenderer( window_, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE );
	// check for errors
	if( renderer_ == NULL )
	{
		std::cout << "Renderer error: " << SDL_GetError( ) << std::endl;
		SDL_Quit();
		std::exit( 1 );
	}

	// Allocate the depth buffer
	depthBuffer_ = new DepthBuffer[width_];
}

Screen::~Screen()
{
	//  cleanup the depth buffer
	if( depthBuffer_ )
	{
		delete[] depthBuffer_;
		depthBuffer_ = nullptr;
	}
	
	SDL_DestroyRenderer( renderer_ );
	SDL_DestroyWindow( window_ );
}

void Screen::Display()
{
	// show the current frame
	SDL_RenderPresent( renderer_ );

	// clear the renderer
	SDL_SetRenderDrawColor( renderer_, 0, 0, 0, 255 );
	SDL_RenderClear( renderer_ );

	// draw sky
	SDL_SetRenderDrawColor( renderer_, 100, 100, 200, 255 );
	SDL_Rect sky = { 0, 0, width_, height_/2 };
	SDL_RenderFillRect( renderer_, &sky );

	// draw ground
	SDL_SetRenderDrawColor( renderer_, 50, 50, 50, 255 );
	SDL_Rect ground = { 0, height_/2, width_, height_/2 };
	SDL_RenderFillRect( renderer_, &ground );


	
}