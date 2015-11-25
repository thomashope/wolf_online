#include "input.h"

Input::Input( ) :
askedToQuit_( false ),
mouse_( 0, 0, SDL_TRUE )
{
	SDL_SetRelativeMouseMode( mouse_.locked );
}

void Input::PorcessEvents()
{
	SDL_Event event;

	// Get the current state of the keyboard
	keyboard_ = SDL_GetKeyboardState( NULL );

	// reset the mouse
	mouse_.xMotion = 0;
	mouse_.yMotion = 0;

	if( keyboard_[SDL_SCANCODE_ESCAPE] )
	{
		askedToQuit_ = true;
	}
	// pressing controll toggles the mouse state
	if( keyboard_[SDL_SCANCODE_LCTRL] && !oldKeyboard_.ctrl )
	{
		mouse_.locked = ((mouse_.locked == SDL_TRUE) ? SDL_FALSE : SDL_TRUE);
		SDL_SetRelativeMouseMode( mouse_.locked );
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
	}

	// store the old keyboard for next frame
	StoreOldKeyboard( );
}

bool Input::KeyDown( SDL_Scancode key )
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

void Input::StoreOldKeyboard()
{
	oldKeyboard_.ctrl = keyboard_[SDL_SCANCODE_LCTRL];
}