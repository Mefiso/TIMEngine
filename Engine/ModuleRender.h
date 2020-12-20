#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float3.h"
#include "MSTimer.h"

class Model;

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	void* context = nullptr;										// Context of the SDL_GL configuration

	bool depthTest = true;											// Set if depth test is performed
	bool cullFace = true;											// Set if face culling is performed
	bool showGrid = true;											// Set if the grid is rendered

	float3 backgroundColor = { 0.1f, 0.1f, 0.1f };					// Base color of the viewport window
	float3 gridColor = { 1.f, 1.f, 1.f };							// Base color of the world Grid

private:
	int viewport_width = 0, viewport_height = 0;					// Initial size of Viewport window
	unsigned int FBO = 0, textureColorbuffer = 0, RBO = 0;			// IDs of the Viewport buffer objects and texture

	MSTimer msTimer;												// Timer object
	float deltatime;												// Time between each frame, in milliseconds

public:
	ModuleRender();													// Constructor
	~ModuleRender();												// Destructor

	//  ----- Module Functions ----- //
	bool Init() override;											// Initialise Module
	update_status PreUpdate() override;								// Operations that must be performed just before each frame
	update_status Update() override;								// Operations performed at each frame
	update_status PostUpdate() override;							// Operations that must be performed just after each frame
	bool CleanUp() override;										// Clean memory allocated by this Module

	// ---------- Getters ---------- //
	unsigned int GetTextureColorbuffer() { return textureColorbuffer; }
	unsigned int GetViewportWidth() { return viewport_width; }
	unsigned int GetViewportHeight() { return viewport_height; }

	// ---------- Setters ---------- //

	// callback funcs
	void ProcessViewportEvents();									// Called from ModuleEditor, when an input is recieved inside the viewport. Perfoms the necessary operations for the corresponding input. This allows the control of the actions performed when the input is captured inside the viewport.

private:
	void InitFramebuffer();											// Initialises a framebuffer to 'FBO', 'RBO' and 'textureColorbuffer' variables
	void TranslateCamera(float deltaTime) const;					// Moves the application camera if an Input event has occurred
	void RotateCameraKeys(float deltaTime) const;					// Rotates the application camera if an Input event has occurred
};
