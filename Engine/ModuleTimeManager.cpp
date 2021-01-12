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
	static float currentFrameTime;
	// Real Time
	currentFrameTime = realTime.Read() / 1000.0f;
	rtDeltaTime = currentFrameTime - lastFrameRTime;
	lastFrameRTime = currentFrameTime;
	// Game Time
	currentFrameTime = (time.Read() / 1000.f);
	deltaTime = (currentFrameTime - lastFrameTime) * timeScale;
	lastFrameTime = currentFrameTime;

	return UPDATE_CONTINUE;
}