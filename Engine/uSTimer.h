#pragma once
#include "SDL.h"

class uSTimer
{
public:
	uSTimer() {}						// Constructor
	~uSTimer() {}						// Destructor
	
	void Start() {						// Starts a precision Timer from the elapsed time of the Execution of the Application.
		start = SDL_GetPerformanceCounter();
		stopped = false;
		elapsed = 0u;
	}
	unsigned int Read() const {			// Reads the current value of the timer
		return (unsigned int)(stopped ? elapsed / (float)freq : (SDL_GetPerformanceCounter() - start) / (float)freq);
	}
	unsigned int Stop() {				// Stops the timer. Returns last the value.
		if (!stopped)
			elapsed = SDL_GetPerformanceCounter() - start;
		stopped = true;
		return (unsigned int)elapsed / (float)freq;
	}

private:
	Uint64	start	= 0u;				// Time value when the timer was started
	Uint64	elapsed	= 0u;				// Stores the value of the elapsed time since the timer was started
	bool	stopped	= true;				// Timer stopped/running
	Uint64	freq	= SDL_GetPerformanceFrequency() / 1000000; // Divisor to return the elapsed time in microseconds.
};