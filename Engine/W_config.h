#pragma once
#include "ImGUI/imgui.h"
#include "Window.h"
#include "ModuleCamera.h"

class WConfig : public Window
{
private:
	float3 Position;
	float MovementSpeed;
	float RotationSpeed;
	float MouseSensitivity;
	// Frustum
	float nearPlane;
	float farPlane;
	float VFOV;
	float aspectRatio;
private:
	void UpdateCamera();

public:
	WConfig(std::string name, int ID);
	~WConfig();

	void Draw() override;
	void UpdateSettings();
};