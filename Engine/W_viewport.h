#pragma once
#include "Window.h"

class WViewport : public Window
{
public:
	WViewport(std::string name);
	~WViewport();

	void Draw() override;

	void SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height);

	unsigned int GetWidth() { return width; }
	unsigned int GetHeight() { return height; }

private:
	unsigned int texid = 0;
	unsigned int width = 0, height = 0;
};