#pragma once
#include "SDL.h"

class MSTimer
{
public:
	MSTimer() {}
	~MSTimer() {}

	void Start()
	{
		start = SDL_GetTicks();
		stopped = false;
		elapsed = 0u;
	}
	unsigned int Read() const
	{
		return stopped ? elapsed : SDL_GetTicks() - start + elapsed;
	}
	unsigned int Stop()
	{
		elapsed += stopped ? 0u : SDL_GetTicks() - start;
		return elapsed;
	}
	void Resume()
	{
		if (stopped)
		{
			start = SDL_GetTicks();
			stopped = false;
		}
	}
	bool IsStopped() const
	{
		return stopped;
	}

private:
	unsigned int start = 0u;
	unsigned int elapsed = 0u;
	bool stopped = true;
};