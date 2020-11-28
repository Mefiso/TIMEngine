#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float4.h"
#include "MSTimer.h"

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
	void WindowResized(unsigned int width, unsigned int height);
	void RotateCameraMouse(float xoffset, float yoffset);
	void MouseWheel(float xoffset, float yoffset);
	void OrbitObject(float xoffset, float yoffset);
	bool DropFile(const std::string& file);
public:
	bool depthTest = true;
	bool cullFace = true;
	void* context = nullptr;
	bool eventOcurred = false;
	bool showGrid = true;

	float4 backgroundColor = { 0.1f, 0.1f, 0.1f, 0.1f };

	unsigned int defaultProgram = 0;

	// Models
	Model* modelLoaded = nullptr;

private:
	void TranslateCamera(float deltaTime);
	void RotateCameraKeys(float deltaTime);

private:
	MSTimer msTimer;
};
