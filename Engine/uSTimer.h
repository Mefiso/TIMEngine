#pragma once
#include "SDL.h"

class uSTimer
{
public:
	uSTimer() {}
	~uSTimer() {}

	void Start() {
		start = SDL_GetPerformanceCounter();
		stopped = false;
		elapsed = 0u;
	}
	unsigned int Read() const {
		return (unsigned int)(stopped ? elapsed / (float)freq : (SDL_GetPerformanceCounter() - start) / (float)freq);
	}
	unsigned int Stop() {
		elapsed = SDL_GetPerformanceCounter() - start;
		return (unsigned int)elapsed;
	}

private:
	Uint64 start = 0u;
	Uint64 elapsed = 0u;
	bool stopped = true;
	Uint64 freq = SDL_GetPerformanceFrequency() * 1000000;
};