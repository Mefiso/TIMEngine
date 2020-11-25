#pragma once
#include "Globals.h"
#include "Window.h"

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

public:
	WConfig(std::string name, int ID);
	~WConfig();

	void Draw() override;
	void UpdateWindowSizeSettings();

private:
	void WindowHeader();
	void RendererHeader();
	void TextureHeader();
	void CameraHeader();
};