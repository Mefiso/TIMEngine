#pragma once
#include "Window.h"
#include <Math/float2.h>


#include "CCamera.h"
#include "GameObject.h"

class WViewport : public Window
{
public:

	bool viewportIsHovered = false;					// Checks if the Viewport is hovered, to check if input events have to be applied to the camera or scene

private:

	unsigned int texid = 0;							// ID of the Texture of the loaded model
	float width = 0, height = 0;					// Size of the viewport
	float2 viewportPosition = float2(0, 0);

public:
	WViewport(std::string name);					// Constructor
	~WViewport();									// Destructor

	void Draw() override;							// Operations performed when Rendering this window

	void SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height); // Called on Initialization, sets this Window size and the image to display from the ColorBuffer and Viewport settings in the Render

	// ---------- Getters ---------- //
	float GetWidth() { return width; }
	float GetHeight() { return height; }
	float2 GetViewportPosition() { return viewportPosition; }



	void DrawGuizmo(CCamera* _camera, GameObject* _go);
};