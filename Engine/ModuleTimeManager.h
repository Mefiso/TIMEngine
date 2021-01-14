#pragma once
#include "Module.h"
#include "MSTimer.h"
#include "uSTimer.h"

class ModuleTimeManager : public Module
{
public:
	uSTimer precisionTimer;
	bool maxFPS = false;

private:
	int frameCount = 0u;
	MSTimer time;
	float timeScale = 1.f;
	float deltaTime = 0.f;
	float lastFrameTime = 0.f;

	MSTimer realTime;
	float rtDeltaTime = 0.f;
	float lastFrameRTime = 0.f;

	float msPerFrame = 33.34f;

public:
	ModuleTimeManager();
	~ModuleTimeManager();

	bool Init() override;
	update_status PreUpdate() override;
	update_status PostUpdate() override;

	// ---------- Getters ---------- //
	int GetFrameCount() const { return frameCount; }
	float GetDeltaTime() const { return deltaTime; }
	float GetRealTimeDeltaTime() const { return rtDeltaTime; }
	float GetTime() const { return (time.Read() / 1000.f) * timeScale; }
	float GetRealTime() const { return realTime.Read() / 1000.f; }
	float GetMSPerFrame() const { return msPerFrame; }

	// ---------- Setters ---------- //
	void SetMSPerFrame(float _msPerFrame) { msPerFrame = _msPerFrame; }
};

