#pragma once
#include "SDL.h"

class MSTimer
{
public:
	MSTimer() {}
	~MSTimer() {}

	void Start() {
		start = SDL_GetTicks();
		stopped = false;
		elapsed = 0u;
	}
	unsigned int Read() const {
		return stopped ? elapsed : SDL_GetTicks() - start;
	}
	unsigned int Stop() {
		elapsed = SDL_GetTicks() - start;
		return elapsed;
	}

private:
	unsigned int start = 0u;
	unsigned int elapsed = 0u;
	bool stopped = true;
};