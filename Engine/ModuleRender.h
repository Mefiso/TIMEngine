#pragma once
#include "Module.h"
#include "Globals.h"

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

	void* context;
	bool eventOcurred = false;

private:
	void TranslateCamera(float deltaTime);
	void RotateCameraKeys(float deltaTime);

private:
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
};
