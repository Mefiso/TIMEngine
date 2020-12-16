#pragma once
#include "Globals.h"
#include "Window.h"
#include "GL/glew.h"
#include <vector>

class WConfig : public Window
{
private:
	// SDL window
	float brightness;					// Screen brightness value passed to the ModuleWindow
	//bool fullscreen;					// Fullscreen enable/disable passed to the ModuleWindow
	bool resizable;						// Resizable enable/disable passed to the ModuleWindow
	bool borderless;					// Borderless enable/disable passed to the ModuleWindow
	bool fulldesktop;					// Fullscreen desktop enable/disable passed to the ModuleWindow
	bool vsync;							// VSYNC enable/disable passed to the ModuleWindow

	//const std::vector<GLint> wrapmode = { GL_REPEAT, GL_CLAMP, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT };																	 // Wrap mode options for the current texture.
	//const std::vector<GLint> filtermode = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR, GL_NEAREST }; // Filter mode options for the current texture.
public:
	WConfig(std::string name);			// Constructor
	~WConfig();							// Destructor

	void Draw() override;

private:
	void WindowHeader();				// Displays ModuleWindow settings and sends them to it when modified.
	void RendererHeader();				// Displays ModuleRender settings and sends them to it when modified.
	//void TextureHeader();				// Displays ModuleTexture settings and sends them to it when modified.
	void CameraHeader();				// Displays ModuleCamera settings and sends them to it when modified.
};