#pragma once
#include "Module.h"
#include "Globals.h"
#include <SDL.h>

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void SendEvent(SDL_Event& event);
private:
	bool show_demo_window = true;
};