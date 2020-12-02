#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Leaks.h"

ModuleWindow::ModuleWindow()
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("[error] SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		//Create window
		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(0, &mode);
		width = mode.w;
		height = mode.h;
		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL;
		
		if(FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		if (MAXIMIZED == true)
		{
			flags |= SDL_WINDOW_MAXIMIZED;
		}
		if (RESIZABLE == true)
			flags |= SDL_WINDOW_RESIZABLE;

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		height = h;
		
		if(window == NULL)
		{
			LOG("[error] Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::ReceiveEvent(const Event& event)
{
	switch (event.type)
	{
	case Event::window_resize:
		width = event.point2d.x;
		height = event.point2d.y;
		break;
	}
}

void ModuleWindow::SetFullscreen(bool fullscreen) const
{
	if (fullscreen)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::SetBorderless(bool borderless) const
{
	if (borderless)
		SDL_SetWindowBordered(window, SDL_FALSE);
	else
		SDL_SetWindowBordered(window, SDL_TRUE);
}

void ModuleWindow::SetFulldesktop(bool fulldesktop) const
{
	if (fulldesktop)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::SetResizable(bool resizable) const
{
	if (resizable)
		SDL_SetWindowResizable(window, SDL_TRUE);
	else
		SDL_SetWindowResizable(window, SDL_FALSE);
}

void ModuleWindow::SetWindowSize() const
{
	SDL_SetWindowSize(window, width, height);
}

void ModuleWindow::SetBrightness(float brightness) const
{
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::SetVsync(bool vsync)
{
	SDL_GL_SetSwapInterval(vsync);
}

