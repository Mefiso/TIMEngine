#pragma once
#include<vector>
#include "Globals.h"
#include "Module.h"

class ModuleWindow;
class ModuleInput;
class ModuleFilesystem;
class ModuleSceneManager;
class ModuleRender;
class ModuleDebugDraw;
class ModuleEditor;
class ModuleCamera;
class ModuleTimeManager;
struct Event;

class Application
{
public:
	// ---- Application Modules ---- //
	ModuleTimeManager* timeMng = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleFilesystem* filesys = nullptr;
	ModuleSceneManager* sceneMng = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleDebugDraw* debugdraw = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleCamera* camera = nullptr;

private:
	std::vector<Module*> modules;						// Vector containig the references to each App Module

public:
	Application();										// Constructor
	~Application();										// Destructor

	bool			Init();								// Call Init() for each Module
	update_status	Update();							// Call Update() for each Module
	bool			CleanUp();							// Call CleanUp() for each Module
	void			BroadcastEvent(const Event& _event);	// Call ReceiveEvent() for each Module

	void Log(const char* input) const;					// Forward a LOG output to the Editor, then to W_Console

	// Easy access to a precision timer from timeManager module
	void			StartTimer();
	unsigned int	ReadTimer() const;
	unsigned int	StopTimer();
};

extern Application* App;
