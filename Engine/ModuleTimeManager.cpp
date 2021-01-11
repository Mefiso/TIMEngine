#include "ModuleTimeManager.h"

ModuleTimeManager::ModuleTimeManager()
{
}

ModuleTimeManager::~ModuleTimeManager()
{
}

bool ModuleTimeManager::Init()
{
	realTime.Start();

	return true;
}

update_status ModuleTimeManager::PreUpdate()
{
	++frameCount;
	rtDeltaTime = realTime.Read() / 1000.f - rtDeltaTime;
	if (!time.IsStopped())
		deltaTime = (time.Read() / 1000.f) * timeScale - deltaTime;
	else
		deltaTime = 0.f;

	return UPDATE_CONTINUE;
}
