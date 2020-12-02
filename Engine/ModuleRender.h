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

	void ReceiveEvent(const Event& event);

	unsigned int GetTextureColorbuffer() { return textureColorbuffer; }
	unsigned int GetViewportWidth() { return viewport_width; }
	unsigned int GetViewportHeight() { return viewport_height; }

	// callback funcs
	void WindowResized(unsigned int width, unsigned int height);
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
	void InitFramebuffer();
	void TranslateCamera(float deltaTime) const;
	void RotateCameraKeys(float deltaTime) const;

private:
	int viewport_width = 0, viewport_height = 0;
	unsigned int FBO = 0, textureColorbuffer = 0, RBO = 0;
	MSTimer msTimer;
};
