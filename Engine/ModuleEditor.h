#pragma once
#include "Module.h"
#include "Globals.h"
#include <SDL.h>
#include "ImGUI/imgui.h"
#include <vector>

class Window;
class WViewport;
class WConsole;
class WMonitor;
class WConfig;
class WProperties;
class WAbout;
class Model;

class ModuleEditor : public Module
{
private:

	bool should_quit = false;										// Boolean setted by the 'Quit' option in the main menu. When TRUE, the application closes
	ImGuiIO* io = nullptr;											// Pointer to the inputs/outputs setting of ImGui

	//  ------ ImGui Windows ------- //
	WViewport* viewport = nullptr;									
	WConsole* console = nullptr;
	WMonitor* monitor = nullptr;
	WConfig* configuration = nullptr;
	WProperties* properties = nullptr;
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
	void ReceiveEvent(const Event& event) override;					// Recieve events from App (that recieves events from other Modules)

	const bool IsViewportHovered() const;							// 


	void SendEvent(const SDL_Event& event) const;					// Processes all ImGui events

	// Windows methods
	void CreateViewport();											// Sends the definition (FBO, RBO, colorBuffer) of the viewport from ModuleRender to W_Viewport
	void Log(const char* input) const;								// Sends LOG inputs from the log function to W_Console
	void ProcessFPS(float deltaTime) const;							// DO IT WITH BROADCAST? OR CHANGE TO ImguiFrameRATE
	

private:
	void SelectedModel() const;				// Sends the information of the loaded model in the render to W_properties (THIS MIGHT BE DEPRECATED)
	void CreateMainMenu();											// Generates the main menu using ImGui

};