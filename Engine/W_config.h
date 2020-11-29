#pragma once
#include "Globals.h"
#include "Window.h"
#include "GL/glew.h"
#include <vector>

class WConfig : public Window
{
private:
	// SDL window
	float brightness;
	int width;
	int height;
	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fulldesktop;
	bool vsync;

	const std::vector<GLint> wrapmode = { GL_REPEAT, GL_CLAMP, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT };
	const std::vector<GLint> filtermode = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR, GL_NEAREST };
public:
	WConfig(std::string name, int ID);
	~WConfig();

	void Draw() override;
	void UpdateWindowSizeSettings();
	
private:
	void WindowHeader();
	void InputHeader();
	void RendererHeader();
	void TextureHeader();
	void CameraHeader();
};