#pragma once
#include "Window.h"

class WViewport : public Window
{
public:

	bool viewportResized = false;					// Checks if a resize event has occurred on the Viewport, to forward the required actions to other Modules
	bool viewportIsHovered = false;					// Checks if the Viewport is hovered, to check if input events have to be applied to the camera or scene

private:

	unsigned int texid = 0;							// ID of the Texture of the loaded model
	unsigned int width = 0, height = 0;				// Size of the viewport


public:
	WViewport(std::string name);					// Constructor
	~WViewport();									// Destructor

	void Draw() override;							// Operations performed when Rendering this window

	void SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height);

	unsigned int GetWidth() { return width; }
	unsigned int GetHeight() { return height; }
};