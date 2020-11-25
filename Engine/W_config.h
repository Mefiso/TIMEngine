#pragma once
#include "Globals.h"
#include "Window.h"
#include "ModuleCamera.h"

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
	
	// Camera
	float3 Position;
	float MovementSpeed;
	float RotationSpeed;
	float MouseSensitivity;
	// Frustum
	float nearPlane;
	float farPlane;
	float VFOV;
	float aspectRatio;

public:
	WConfig(std::string name, int ID);
	~WConfig();

	void Draw() override;
	void UpdateCameraSettings();
	void UpdateWindowSizeSettings();

private:
	void WindowHeader();
	void RendererHeader();
	void TextureHeader();
	void CameraHeader();
};