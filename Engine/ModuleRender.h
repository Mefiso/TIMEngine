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

	int viewport_width = 0, viewport_height = 0;
	unsigned int FBO = 0, textureColorbuffer = 0, RBO = 0;
	MSTimer msTimer;
	float deltatime;


public:
	ModuleRender();													// Constructor
	~ModuleRender();												// Destructor

	//  ----- Module Functions ----- //
	bool Init() override;											// Initialise ModuleWindow
	update_status PreUpdate() override;								// Operations that must be performed just before each frame
	update_status Update() override;								// Operations performed at each frame
	update_status PostUpdate() override;							// Operations that must be performed just after each frame
	bool CleanUp() override;										// Clean memory allocated by this Module
	void ReceiveEvent(const Event& event) override;					// Recieve events from App (that recieves events from other Modules)

	// ---------- Getters ---------- //
	unsigned int GetTextureColorbuffer() { return textureColorbuffer; }
	unsigned int GetViewportWidth() { return viewport_width; }
	unsigned int GetViewportHeight() { return viewport_height; }

	// ---------- Setters ---------- //
	void SetViewportWidth(unsigned int _width) { viewport_width = _width; }
	void SetViewportHeight(unsigned int _height) { viewport_height = _height; }

	// callback funcs
	void WindowResized(unsigned int width, unsigned int height);	// Called upon recieving a Resizing event. Forwards the new aspect ratio to the application Camera
	bool DropFile(const std::string& file);							// Called upon recieving a Drag&Drop event. Replaces the current Model for the new one if 'file' is an .fbx.
	void ProcessViewportEvents();

private:
	void InitFramebuffer();											// Initialises a framebuffer to 'FBO', 'RBO' and 'textureColorbuffer' variables
	void TranslateCamera(float deltaTime) const;					// Moves the application camera if an Input event has occurred
	void RotateCameraKeys(float deltaTime) const;					// Rotates the application camera if an Input event has occurred

};
