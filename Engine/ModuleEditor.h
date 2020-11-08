#pragma once
#include "Module.h"
#include "Globals.h"
#include <SDL.h>
#include "ImGUI/imgui.h"

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
	ImGuiIO* io = nullptr;
	bool show_demo_window = true;
};