#pragma once
#include<vector>
#include "Globals.h"
#include "Module.h"
#include "Event.h"

class ModuleWindow;
class ModuleInput;
class ModuleScene;
class ModuleRender;
class ModuleDebugDraw;
class ModuleEditor;
class ModuleCamera;

class Application
{
public:
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleScene* scene = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleDebugDraw* debugdraw = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleCamera* camera = nullptr;

private:

	std::vector<Module*> modules;


public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void BroadcastEvent(const Event& event);

	void Log(const char* input) const;
};

extern Application* App;
