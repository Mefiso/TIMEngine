#pragma once
#include "SDL.h"

class MSTimer
{
public:
	MSTimer() {}						// Constructor
	~MSTimer() {}						// Destructor

	void Start()						// Starts a milliseconds Timer from the elapsed Ticks of the Execution of the Application
	{
		start = SDL_GetTicks();
		stopped = false;
		elapsed = 0u;
	}
	unsigned int Read() const			// Reads the current value of the timer
	{
		return stopped ? elapsed : SDL_GetTicks() - start + elapsed;
	}
	unsigned int Stop()					// Stops the timer and resets to 0. Returns last the value.
	{
		elapsed += stopped ? 0u : SDL_GetTicks() - start;
		stopped = true;
		return elapsed;
	}
	void Resume()						// Continues the counting of this timer.
	{
		if (stopped)
		{
			start = SDL_GetTicks();
			stopped = false;
		}
	}
	bool IsStopped() const				// Checks if the timer was Stopped
	{
		return stopped;
	}

private:
	unsigned int	start	= 0u;		// Tick value when the timer was started
	unsigned int	elapsed	= 0u;		// Stores the value of the elapsed ticks since the timer was started
	bool			stopped	= true;		// Timer stopped/running
};