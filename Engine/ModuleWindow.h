#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	float brightness = 1.0f;						// Gamma value of the screen
	SDL_Window* window = NULL;						// The window we'll be rendering to
	SDL_Surface* screen_surface = NULL;				// The surface contained by the window

private:

	int width, height;								// Size of the application Window

public:
	ModuleWindow();									// Constructor
	virtual ~ModuleWindow();						// Destructor

	//  ----- Module Functions ----- //
	bool Init() override;							// Initialise ModuleWindow
	bool CleanUp() override;						// Clean memory allocated by this Module
	void ReceiveEvent(const Event& event) override;	// Recieve events from App (that recieves events from other Modules)

	// ---------- Getters ---------- //
	int GetWindowWidth() { return width; }
	int* GetWindowWidthPtr() { return &width; }
	int GetWindowHeight() { return height; }
	int* GetWindowHeightPtr() { return &height; }

	// ---------- Setters ---------- //
	void ToggleBorderless(bool borderless) const;	// Toggles the Borderless mode of the application Window
	void ToggleFulldesktop(bool fulldesktop) const;	// Toggles the Fulldesktop mode of the application Window (resizes the window to maximum screen resolution)
	void ToggleResizable(bool resizable) const;		// Toggles the Resizable option of the application Window
	void SetBrightness(float brightness) const;		// Sets the gamma value of the screen
	void SetVsync(bool vsync);						// Toggles the VSync of the application Window
};

#endif // __ModuleWindow_H__