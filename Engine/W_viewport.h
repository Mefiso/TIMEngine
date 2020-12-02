#pragma once
#include "Window.h"

class WViewport : public Window
{
public:

	bool viewportResized = false;
	bool viewportIsHovered = false;

private:

	unsigned int texid = 0;
	unsigned int width = 0, height = 0;


public:
	WViewport(std::string name);
	~WViewport();

	void Draw() override;

	void SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height);

	unsigned int GetWidth() { return width; }
	unsigned int GetHeight() { return height; }
};