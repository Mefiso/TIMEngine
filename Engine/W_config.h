#pragma once
#include "Globals.h"
#include "Window.h"
#include "GL/glew.h"
#include <vector>

class WConfig : public Window
{
private:
	float	brightness;				// Screen brightness value passed to the ModuleWindow
	bool	resizable;				// Resizable enable/disable passed to the ModuleWindow
	bool	borderless;				// Borderless enable/disable passed to the ModuleWindow
	bool	fulldesktop;			// Fullscreen desktop enable/disable passed to the ModuleWindow
	bool	vsync;					// VSYNC enable/disable passed to the ModuleWindow

public:
	WConfig(std::string _name);		// Constructor
	~WConfig();						// Destructor

	void Draw() override;			// Operations performed when Rendering this window

private:
	void DrawWindowHeader();		// Displays ModuleWindow settings and sends them to it when modified.
	void DrawRendererHeader();		// Displays ModuleRender settings and sends them to it when modified.
	void DrawCameraHeader();		// Displays ModuleCamera settings and sends them to it when modified.
};