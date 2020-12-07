#pragma once
#include<vector>
#include "Globals.h"
#include "Module.h"
#include "Event.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleCamera;
//class ModuleProgram;
class ModuleEditor;
class ModuleDebugDraw;
//class ModuleTexture;

class Application
{
public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleCamera* camera = nullptr;
	//ModuleProgram* program = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleDebugDraw* debugdraw = nullptr;
	//ModuleTexture* textureLoader = nullptr;

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
