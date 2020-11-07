#pragma once
#include "Module.h"
#include "Globals.h"

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

private:
	const Uint8 *keyboard = NULL;
	KeyState* keyboard_state = nullptr;

};