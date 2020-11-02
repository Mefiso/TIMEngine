#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "SDL.h"

ModuleCamera::ModuleCamera(float3 position, float3 up, float yaw, float pitch) : 
	Front(-float3::unitZ), MovementSpeed(SPEED), MouseSensitivity(SENSIIVITY), Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch)
{
}

// Destructor
ModuleCamera::~ModuleCamera()
{
}

// Called before Camera is available
bool ModuleCamera::Init()
{
	Right = Cross(Front, WorldUp).Normalized();
	Up = Cross(Right, Front).Normalized();

	return true;
}

// Called every draw update
update_status ModuleCamera::Update()
{
	

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	LOG("Destroying Camera");

	return true;
}