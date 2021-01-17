#pragma once
#include "Module.h"
#include "SDL.h"

class Application;

class ModuleWindow : public Module
{
public:
	float			brightness		= 1.0f;				// Gamma value of the screen
	SDL_Window*		window			= nullptr;			// The window we'll be rendering to
	SDL_Surface*	screen_surface	= nullptr;			// The surface contained by the window

private:

	int width = 0, height = 0;							// Size of the application Window

public:
	ModuleWindow();										// Constructor
	virtual ~ModuleWindow();							// Destructor

	//  ----- Module Functions ----- //
	bool Init() override;								// Initialise ModuleWindow
	bool CleanUp() override;							// Clean memory allocated by this Module
	void ReceiveEvent(const Event& _event) override;		// Recieve events from App (that recieves events from other Modules)

	// ---------- Getters ---------- //
	int GetWindowWidth() const	{ return width; }
	int GetWindowHeight() const	{ return height; }

	// ---------- Setters ---------- //
	void SetBrightness(float _brightness)		{ SDL_SetWindowBrightness(window, _brightness); }										// Sets the gamma value of the screen
	void SetVsync(bool _vsync)					{ SDL_GL_SetSwapInterval(_vsync); }														// Toggles the VSync of the application Window
	void ToggleBorderless(bool _borderless)		{ SDL_SetWindowBordered(window, _borderless ? SDL_FALSE : SDL_TRUE); }					// Toggles the Borderless mode of the application Window
	void ToggleFulldesktop(bool _fulldesktop)	{ SDL_SetWindowFullscreen(window, _fulldesktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0); }	// Toggles the Fulldesktop mode of the application Window (resizes the window to maximum screen resolution)
	void ToggleResizable(bool _resizable)		{ SDL_SetWindowResizable(window, _resizable ? SDL_TRUE : SDL_FALSE); }					// Toggles the Resizable option of the application Window
};