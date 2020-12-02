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

	void ReceiveEvent(const Event& event);

	// Getters
	int GetWindowWidth() { return width; }
	int GetWindowHeight() { return height; }

	void SetFullscreen(bool fullscreen) const;
	void SetBorderless(bool borderless) const;
	void SetFulldesktop(bool fulldesktop) const;
	void SetResizable(bool resizable) const;
	void SetWindowSize() const;
	void SetBrightness(float brightness) const;
	void SetVsync(bool vsync);
public:
	float brightness = 1.0f;
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;

private:
	int width, height;
};

#endif // __ModuleWindow_H__