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
		return stopped ? elapsed / (float)freq : (SDL_GetPerformanceCounter() - start) / (float)freq;
	}
	unsigned int Stop() {
		elapsed = SDL_GetPerformanceCounter() - start;
		return elapsed;
	}

private:
	unsigned int start = 0u;
	unsigned int elapsed = 0u;
	bool stopped = true;
	unsigned int freq = SDL_GetPerformanceFrequency() * 1000000;
};