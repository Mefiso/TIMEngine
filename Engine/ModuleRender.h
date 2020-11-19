#pragma once
#include "Module.h"
#include "Globals.h"

class Model;

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	// callback funcs
	void WindowResized(unsigned width, unsigned height);
	void RotateCameraMouse(float xoffset, float yoffset);
	void MouseWheel(float xoffset, float yoffset);
	void DropFile(const char* file);
public:
	void* context = nullptr;
	bool eventOcurred = false;

	unsigned int defaultProgram;

	// Models
	Model* bakerHouse = nullptr;

private:
	void TranslateCamera(float deltaTime);
	void RotateCameraKeys(float deltaTime);

private:
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
};
