#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float3.h"

// Default values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSIIVITY = .1f;

class ModuleCamera : public Module
{
public:
	ModuleCamera(float3 position = float3(0, 1, 4), float3 up = float3(0, 1, 0), float yaw = YAW, float pitch = PITCH);
	~ModuleCamera();

	bool Init();
	update_status Update();
	bool CleanUp();
public:
	float3 Position;
	float3 Front;
	float3 Up;
	float3 Right;
	float3 WorldUp;
	// Euler angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
};