#include "Globals.h"
#include "Application.h"
#include "Event.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "SDL/include/SDL.h"
#include "Leaks.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput() : Module()
{
	keyboard_state = new KeyState[MAX_KEYS];
	memset(keyboard_state, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("[error] SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
	SDL_Event sdlEvent;

	memset(windowEvents, false, WE_COUNT * sizeof(bool));

	keyboard = SDL_GetKeyboardState(NULL);

	// Update each key state
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i] == 1)
		{
			if (keyboard_state[i] == KEY_IDLE)
				keyboard_state[i] = KEY_DOWN;
			else
				keyboard_state[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard_state[i] == KEY_REPEAT || keyboard_state[i] == KEY_DOWN)
				keyboard_state[i] = KEY_UP;
			else
				keyboard_state[i] = KEY_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if (mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			windowEvents[WE_QUIT] = true;
		case SDL_WINDOWEVENT:
			switch (sdlEvent.window.event)
			{
				//case SDL_WINDOWEVENT_LEAVE:
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_FOCUS_LOST:
				windowEvents[WE_HIDE] = true;
				break;

				//case SDL_WINDOWEVENT_ENTER:
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:
				windowEvents[WE_SHOW] = true;
				break;
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				/*Event ev(Event::window_resize);
				ev.point2d.x = sdlEvent.window.data1;
				ev.point2d.y = sdlEvent.window.data2;
				App->BroadcastEvent(ev);*/
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			if (App->editor->IsViewportHovered()) {
				if (GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
					if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
						SDL_SetRelativeMouseMode(SDL_TRUE);
					}
					Event ev(Event::rotate_event);
					ev.point2d.x = sdlEvent.motion.xrel;
					ev.point2d.y = -sdlEvent.motion.yrel;
					App->BroadcastEvent(ev);
				}
				else if (GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
					if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
						SDL_SetRelativeMouseMode(SDL_TRUE);
					}
					Event ev(Event::orbit_event);
					ev.point2d.x = -sdlEvent.motion.xrel;
					ev.point2d.y = -sdlEvent.motion.yrel;
					App->BroadcastEvent(ev);
				}
			}
			break;
		case SDL_MOUSEWHEEL:
		{
			if (App->editor->IsViewportHovered()) {
				Event ev(Event::wheel_event);
				ev.point2d.x = sdlEvent.wheel.x;
				ev.point2d.y = sdlEvent.wheel.y;
				App->BroadcastEvent(ev);
			}
			break;
		}
		case SDL_DROPFILE:
		{
			LOG("[info] A file has been dropped %s", sdlEvent.drop.file);
			Event ev(Event::file_dropped);
			ev.string.ptr = sdlEvent.drop.file;
			App->BroadcastEvent(ev);
			App->editor->SelectedModel(App->renderer->modelLoaded);
			SDL_free(sdlEvent.drop.file);
			break; 
		}
		case SDL_MOUSEBUTTONDOWN:
			mouse_buttons[sdlEvent.button.button - 1] = KEY_DOWN;
			break;
		case SDL_MOUSEBUTTONUP:
			mouse_buttons[sdlEvent.button.button - 1] = KEY_UP;
			break;
		}

		App->editor->SendEvent(sdlEvent);
	}

	if (GetWindowEvent(EventWindow::WE_QUIT) == true || (GetKey(SDL_SCANCODE_LALT) == KEY_DOWN && GetKey(SDL_SCANCODE_F4) == KEY_DOWN))
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	RELEASE(keyboard_state);

	return true;
}
