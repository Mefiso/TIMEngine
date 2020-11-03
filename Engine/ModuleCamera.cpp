#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "SDL.h"

ModuleCamera::ModuleCamera()
{
}

// Destructor
ModuleCamera::~ModuleCamera()
{
}

// Called before Camera is available
bool ModuleCamera::Init()
{
	
	return true;
}

update_status ModuleCamera::PreUpdate()
{

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleCamera::Update()
{

	return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	LOG("Destroying Camera");

	//Destroy window

	return true;
}