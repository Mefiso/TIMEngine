#pragma once
#include "Module.h"
#include "Globals.h"

#define NUM_MOUSE_BUTTONS 5
typedef unsigned __int8 Uint8;

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
private:

	bool windowEvents[WE_COUNT];													// Array of booleant that incidate the program state of the application
	const Uint8* keyboard = NULL;													// Pointer to an array that stores the definition of each key in the user keyboard
	KeyState* keyboard_state = nullptr;												// Pointer to an array that stores the KeyState for each key on 'keyboard'
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];										// Array that stores the KeyState for each mouse button


public:
	
	ModuleInput();																	// Constructor
	~ModuleInput();																	// Destructor

	//  ----- Module Functions ----- //
	bool Init() override;															// Initialise ModuleWindow
	// Operations that must be performed just before each frame
	update_status PreUpdate() override;												// Operations that must be performed just before each frame
	bool CleanUp() override;														// Clean memory allocated by this Module

	// ---------- Getters ---------- //
	KeyState GetKey(int id) const { return keyboard_state[id]; }					// Returns the keyboard inputs produced last frame
	KeyState GetMouseButtonDown(int id) const { return mouse_buttons[id - 1]; }		// Returns the mouse inputs produced last frame
	bool GetWindowEvent(EventWindow ev) const { return windowEvents[ev]; }			// Returns the window events produced last frame

};