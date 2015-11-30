#include "input.h"
#include <iostream>

Input::Input( ) :
askedToQuit_( false ),
mouse_( 0, 0, SDL_TRUE )
{
	SDL_SetRelativeMouseMode( mouse_.locked );
	keyboard_ = SDL_GetKeyboardState( NULL );
}

void Input::PorcessEvents()
{
	SDL_Event event;

	// store the old keyboard for next frame
	StoreKeyboard();
	// Get the current state of the keyboard
	keyboard_ = SDL_GetKeyboardState( NULL );

	// reset the mouse
	mouse_.xMotion = 0;
	mouse_.yMotion = 0;

	if( keyboard_[SDL_SCANCODE_ESCAPE] )
	{
		askedToQuit_ = true;
	}

	while( SDL_PollEvent( &event ) )
	{
		// quit if there is a quit message or escape is pressed
		if( event.type == SDL_QUIT )
		{
			askedToQuit_ = true;
		}
		// update the mouse struct if it moved
		if( event.type == SDL_MOUSEMOTION )
		{
			HandleMouseEvent( event );
		}
		// toggle mouse lock with Left CTRL key
		if( event.type == SDL_KEYUP )
		{
			if( event.key.keysym.scancode == SDL_SCANCODE_LCTRL )
			{
				mouse_.locked = ((mouse_.locked == SDL_TRUE) ? SDL_FALSE : SDL_TRUE);
				SDL_SetRelativeMouseMode( mouse_.locked );
			}
		}
	}
}

bool Input::KeyDown( SDL_Scancode key ) const
{
	return keyboard_[key];
}

void Input::HandleMouseEvent(const SDL_Event& mouseEvent)
{
	if( mouse_.locked )
	{
		mouse_.xMotion += mouseEvent.motion.xrel;
		mouse_.yMotion += mouseEvent.motion.yrel;
	}
}

void Input::StoreKeyboard( )
{
	oldKeyboard_.ctrl = (bool)keyboard_[SDL_SCANCODE_LCTRL];
}