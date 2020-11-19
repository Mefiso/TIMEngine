#pragma once
#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleCamera;
class ModuleProgram;
//class ModuleRenderExercise;
class ModuleEditor;
class ModuleDebugDraw;
class ModuleTexture;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void Log(const char* input);

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleProgram* program = nullptr;
	ModuleEditor* editor = nullptr;
	//ModuleRenderExercise* exercise = nullptr;
	ModuleDebugDraw* debugdraw = nullptr;
	ModuleTexture* textureLoader = nullptr;

private:

	std::list<Module*> modules;

};

extern Application* App;
