#pragma once
#include <vector>
#include "Window.h"
#include "SDL.h"

struct Hardware {
	SDL_version compiled;
	int CPU_count;
	int CPU_cache;
	float RAM;
	std::vector<const char*> caps = { "3DNow", "AVX", "AVX2", "AltiVec",
									"MMX", "RDTSC", "SSE", "SSE2",
									"SSE3", "SSE41", "SSE42" };
	SDL_bool has_caps[11] = {SDL_Has3DNow(), SDL_HasAVX(), SDL_HasAVX2(),
	SDL_HasAltiVec(), SDL_HasMMX(), SDL_HasRDTSC(), SDL_HasSSE(),
	SDL_HasSSE2(), SDL_HasSSE3(), SDL_HasSSE41(), SDL_HasSSE42()};
};

class WMonitor : public Window
{
private:

	Hardware hardware;								// System hardware Information
	float fpsNow = 0.f;								// Framerate calculated of the current frame
	int frames = 0;									// Total frames elapsed since the start of the Application
	float elapsedTime = 0.0f;						// Total time elapsed since the satart of the Application
	char title[75];									// Title of the Framerate Histogram
	std::vector<float> fps_log;						// Vector of deltatimes that will be represented in the Framerate Histogram
	const unsigned int histNumElements = 50u;		// Maximum number of elements represented in the Framerate Histogram


public:

	WMonitor(std::string name);						// Constructor
	~WMonitor();									// Destructor
	void Draw() override;							// Operations performed when Rendering this window

private:

	void DrawInputHeader();								// Prints the Information of the inputs performed at each frame
	void ShowSoftware() const;						// Prints the Information of the Software used to build this Application
	void ShowHardware() const;						// Prints the Information of the iHardware of your Computer

};