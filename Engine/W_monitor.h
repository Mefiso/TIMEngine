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
	char title[37];
	int log_size = 100;
	std::vector<float> fps_log;
	std::vector<float> ms_log;
	Hardware hardware;
	int frames = 0;
	float elapsedTime = 0.0f;
	float fpsNow = 0.f;
	const unsigned int histNumElements = 50u;

public:
	WMonitor(std::string name);
	~WMonitor();

	void Draw() override;
	void AddFPS(float deltaTime);
private:
	void InputHeader();
	void ShowSoftware() const;
	void ShowHardware() const;

};