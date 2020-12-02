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
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void ReceiveEvent(const Event& event);

	void SendEvent(const SDL_Event& event) const;

	// Windows methods
	void CreateViewport();
	void Log(const char* input) const;
	void ProcessFPS(float deltaTime) const;
	void SelectedModel(const Model* const model) const;
private:
	void CreateMainMenu();
private:
	bool should_quit = false;
	ImGuiIO* io = nullptr;
	
	WViewport* viewport = nullptr;
	WConsole* console = nullptr;
	WMonitor* monitor = nullptr;
	WConfig* configuration = nullptr;
	WProperties* properties = nullptr;
	WAbout* about = nullptr;

	std::vector<Window*> editorWindows;
};