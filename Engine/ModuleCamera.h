#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL.h"
#include "Geometry/Frustum.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	PITCH_UP,
	PITCH_DOWN,
	YAW_LEFT,
	YAW_RIGHT
};

// Default values
const float SPEED = 2.5f;
const float ROTATION_SPEED = (float)(M_PI / 8.0f);
const float SENSITIVITY = .005f;
const float VERTICALFOV = DegToRad(60.f);
const float ASPECTRATIO = SCREEN_WIDTH / (float) SCREEN_HEIGHT;
const float ZOOM = .1f;

class ModuleCamera : public Module
{
public:
	ModuleCamera(float3 position = float3(0, 1, 7), float3 up = float3(0, 1, 0), float near_plane = 0.1f, float far_plane = 200.0f);
	~ModuleCamera();

	bool CleanUp();

	float4x4 ViewMatrix();
	float4x4 ProjectionMatrix();

	// Process movement
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessMouseScroll(float yoffset);
	void ProcessSpeed(float multiplier);
	void ProcessOrbit(float xoffset, float yoffset, float3 orbit_centre);
	void onResize(float aspect_ratio);
	void onFocus(float3 center, float distance);

public:
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float RotationSpeed;
	Frustum frustum;
private:
	void RotateCamera(float yaw, float pitch);
};