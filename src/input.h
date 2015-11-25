#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>


class Input {
public:
	Input();
	~Input() {}

	void PorcessEvents();

	bool KeyDown(SDL_Scancode key) const;

	bool AskedToQuit() const { return askedToQuit_; }

	// TODO: change this so it's handled inside the player, friend of player?
	int XMotion() const { return mouse_.xMotion; }

private:

	void HandleMouseEvent( const SDL_Event& mouseEvent );

	const Uint8* keyboard_;

	// only stores info for keys we want to detect 'presses' for
	struct OldKeyboard {
		OldKeyboard( ) :ctrl( false ){}
		bool ctrl;
	} oldKeyboard_;

	// save the current state of keys in the old keyboard
	void StoreKeyboard( );

	struct Mouse {
		Mouse( int x, int y, SDL_bool l ) :xMotion( x ), yMotion( y ), locked( l ){}
		int xMotion;
		int yMotion;
		SDL_bool locked;
	} mouse_;

	// true if the user asked to quit the application
	bool askedToQuit_;
};

#endif