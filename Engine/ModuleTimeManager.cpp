#include "ModuleTimeManager.h"
#include "SDL.h"
#include "Leaks.h"

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

update_status ModuleTimeManager::PostUpdate()
{
	if (maxFPS)
	{
		static float elapsed;
		elapsed = realTime.Read() - lastFrameRTime * 1000.0f;
		if (elapsed < msPerFrame)
			SDL_Delay(msPerFrame - elapsed);
	}

	return UPDATE_CONTINUE;
}
