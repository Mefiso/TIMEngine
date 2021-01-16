#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <Math/float2.h>
#include "W_viewport.h"
#include "W_tools.h"
class Window;
class WConsole;
class WHierarchy;
class WMonitor;
class WConfig;
class WProperties;
class WAbout;
class Model;
class GameObject;
struct ImGuiIO;
union SDL_Event;

class ModuleEditor : public Module
{
private:
	bool should_quit = false;										// Boolean setted by the 'Quit' option in the main menu. When TRUE, the application closes
	ImGuiIO* io = nullptr;											// Pointer to the inputs/outputs setting of ImGui

	//  ------ ImGui Windows ------- //
	WConfig* configuration = nullptr;
	WConsole* console = nullptr;
	WHierarchy* hierarchy = nullptr;
	WMonitor* monitor = nullptr;
	WProperties* properties = nullptr;
	WViewport* viewport = nullptr;
	WTools* tools = nullptr;
	WAbout* about = nullptr;

	std::vector<Window*> editorWindows;								// Vector containig the references to each ImGui window

public:
	ModuleEditor();													// Constructor
	~ModuleEditor();												// Destructor

	//  ----- Module Functions ----- //
	bool Init() override;											// Initialise ModuleWindow
	update_status PreUpdate() override;								// Operations that must be performed just before each frame
	update_status Update() override;								// Operations performed at each frame
	update_status PostUpdate() override;							// Operations that must be performed just after each frame
	bool CleanUp() override;										// Clean memory allocated by this Module

	const bool IsViewportHovered() const;							// Checks if the W_Viewport is hovered to allow the process of mouse inputs. Called from ModuleInput

	void SendEvent(const SDL_Event& event) const;					// Processes all ImGui events

	// Windows methods
	void CreateViewport();											// Sends the definition (FBO, RBO, colorBuffer) of the viewport from ModuleRender to W_Viewport
	void Log(const char* input) const;								// Sends LOG inputs from the log function to W_Console

	// ---------- Getters ---------- //
	float2 GetViewportPos() const { return viewport->GetViewportPosition(); }
	float2 GetViewportSize() const { return float2(viewport->GetWidth(), viewport->GetHeight()); }
	int GetGuizmoState() const { return tools->GetGuizmoState(); }
	int GetGuizmoMode() const { return tools->GetGuizmoMode(); }
	int GetPlayPauseState() const { return tools->GetPlayPauseState(); }

private:
	void DrawMainMenu();											// Generates the main menu using ImGui
	void ShowMenuFile();											// Subfunction of CreateMainMenu. Generates the File menu
	void ShowMenuGameObject();										// Subfunction of CreateMainMenu. Generates the GameObject menu
};