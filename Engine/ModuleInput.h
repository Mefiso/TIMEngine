#pragma once
#include "Module.h"
#include "Globals.h"

#define NUM_MOUSE_BUTTONS 5
typedef unsigned __int8 Uint8;

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status PreUpdate();
	bool CleanUp();

	KeyState GetKey(int id) const
	{
		return keyboard_state[id];
	}
	KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

private:
	const Uint8 *keyboard = NULL;
	KeyState* keyboard_state = nullptr;
	KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];

};