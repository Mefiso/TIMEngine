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
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float ROTATION_SPEED = 20.f;
const float SENSITIVITY = .1f;
const float HORIZONTALFOV = DegToRad(90.f);
const float ASPECTRATIO = (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT;

class ModuleCamera : public Module
{
public:
	ModuleCamera(float3 position = float3(0, 1, 4), float3 up = float3(0, 1, 0), float yaw = YAW, float pitch = PITCH,
	float near_plane = 0.1f, float far_plane = 200.0f);
	~ModuleCamera();

	bool Init();
	update_status PreUpdate();
	bool CleanUp();

	float4x4 ViewMatrix();
	float4x4 ProjectionMatrix();

	// Process movement
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessMouseScroll(float yoffset);

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
	float RotationSpeed;
	// Frustum
	float nearPlane;
	float farPlane;
	float HFOV;
	float VFOV;
	float aspectRatio;
private:
	Frustum frustum;
	void UpdateFrustum();
	void RotateCamera(float3& axis, float angle);
	void NewDirection();
};