#pragma once
#include<vector>
#include "Globals.h"
#include "Module.h"

class ModuleWindow;
class ModuleInput;
class ModuleScene;
class ModuleRender;
class ModuleDebugDraw;
class ModuleEditor;
class ModuleCamera;
struct Event;

class Application
{
public:
	// ---- Application Modules ---- //
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleScene* scene = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleDebugDraw* debugdraw = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleCamera* camera = nullptr;

private:
	std::vector<Module*> modules;				// Vector containig the references to each App Module

public:
	Application();								// Constructor
	~Application();								// Destructor

	bool Init();								// Call Init() for each Module
	update_status Update();						// Call Update() for each Module
	bool CleanUp();								// Call CleanUp() for each Module
	void BroadcastEvent(const Event& event);	// Call ReceiveEvent() for each Module

	void Log(const char* input) const;			// Forward a LOG output to the Editor, then to W_Console
};

extern Application* App;
