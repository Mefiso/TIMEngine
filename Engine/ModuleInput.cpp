#include "Globals.h"
#include "Application.h"
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
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
	SDL_Event sdlEvent;
	ImGuiIO& io = ImGui::GetIO();
	bool imguiHasInputs = io.WantCaptureMouse || io.WantCaptureKeyboard;

	keyboard = SDL_GetKeyboardState(NULL);

	if (!imguiHasInputs)
	{// Update each key state
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
		
		if (!imguiHasInputs)//sdlEvent.window.windowID == SDL_GetWindowID(App->window->window))
		{
			App->renderer->eventOcurred = true;
			switch (sdlEvent.type)
			{
			case SDL_QUIT:
				return UPDATE_STOP;
			case SDL_WINDOWEVENT:
				if (sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE)
					return UPDATE_STOP;
				if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)// || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					App->renderer->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
				break;
			case SDL_MOUSEMOTION:
				if (GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
					App->renderer->RotateCameraMouse(sdlEvent.motion.xrel, -sdlEvent.motion.yrel);
				break;
			case SDL_MOUSEWHEEL:
				App->renderer->MouseWheel(sdlEvent.wheel.x, sdlEvent.wheel.y);
				break;
			case SDL_DROPFILE:
				LOG(sdlEvent.drop.file); 
				if (App->renderer->DropFile(sdlEvent.drop.file)) {
					App->editor->SelectedModel(App->renderer->modelLoaded);
				}
				SDL_free(sdlEvent.drop.file);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[sdlEvent.button.button - 1] = KEY_DOWN;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse_buttons[sdlEvent.button.button - 1] = KEY_UP;
				break;
			}
		}
		else {
			App->editor->SendEvent(sdlEvent);
			App->renderer->eventOcurred = false;
		}
	}


	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	delete keyboard_state;

	return true;
}
