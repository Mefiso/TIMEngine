#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

	// Called before quitting
	bool Init();

	// Called before quitting
	bool CleanUp();

	void SetFullscreen(const bool fullscreen);
	void SetBorderless(const bool borderless);
	void SetFulldesktop(const bool fulldesktop);
	void SetResizable(const bool resizable);
	void SetWindowSize(const float width, const float height);
	void SetBrightness(const float brightness);
public:
	int width, height;
	float brightness = 1.0f;
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;
};

#endif // __ModuleWindow_H__