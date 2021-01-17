#pragma once
#include "Module.h"
#include "MSTimer.h"
#include "uSTimer.h"

class ModuleTimeManager : public Module
{
public:
	uSTimer precisionTimer;
	bool	maxFPS = false;

private:
	int		frameCount = 0u;		// App graphics frames since game start
	float	msPerFrame = 33.34f;	// Maximum framerate allowed when limiting the FPS (inverse of maxFPS)

	// Game Clock
	MSTimer	time;					// InGame time clock
	float	timeScale		= 1.f;	// Scale at which time is passing
	float	deltaTime		= 0.f;	// Frame time expressed in seconds
	float	lastFrameTime	= 0.f;	// Last frame time expressed in seconds

	// Real Time Clock
	MSTimer	realTime;				// Application clock
	float	rtDeltaTime		= 0.f;	// Frame time expressed in seconds
	float	lastFrameRTime	= 0.f;	// Last frame time expressed in seconds

public:
	ModuleTimeManager();			// Constructor
	~ModuleTimeManager();			// Destructor

	//  ----- Module Functions ----- //
	bool Init()	override;					// Initialise Module
	update_status PreUpdate() override;		// Operations that must be performed just before each frame

	// Easy access to a precision timer for the Game Clock
	void StartGameClock() { time.Start(); }
	void StopGameClock() { time.Stop(); }
	void ResumeGameClock() { time.Resume(); }

	// ---------- Getters ---------- //
	int		GetFrameCount() const			{ return frameCount; }
	float	GetDeltaTime() const			{ return deltaTime; }
	float	GetRealTimeDeltaTime() const	{ return rtDeltaTime; }
	float	GetTime() const					{ return (time.Read() / 1000.f) * timeScale; }
	float	GetRealTime() const				{ return realTime.Read() / 1000.f; }
	float	GetMSPerFrame() const			{ return msPerFrame; }

	// ---------- Setters ---------- //
	void SetMSPerFrame(float _msPerFrame) { msPerFrame = _msPerFrame; }
};

