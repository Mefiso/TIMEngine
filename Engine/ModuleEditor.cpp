#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ImGUI/imgui_impl_sdl.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include "W_console.h"
#include "W_monitor.h"
#include "W_config.h"
#include "W_about.h"
#include "W_properties.h"
#include "Leaks.h"

ModuleEditor::ModuleEditor()
{
	editorWindows.push_back(console = new WConsole("Console", 0));
	editorWindows.push_back(monitor = new WMonitor("Monitoring window", 1));
	editorWindows.push_back(configuration = new WConfig("Configuration", 2));
	editorWindows.push_back(new WAbout("About", 3));
	editorWindows.push_back(properties = new WProperties("Properties", 4));
}

ModuleEditor::~ModuleEditor()
{
	
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	//io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	io->ConfigViewportsNoAutoMerge = true;
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init();

	SelectedModel(App->renderer->modelLoaded);

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	ImGui::ShowDemoWindow();
	
	for (std::vector<Window*>::iterator it = editorWindows.begin(), end = editorWindows.end(); it != end; ++it)
	{
		if ((*it)->isEnabled())
			(*it)->Draw();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	   // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	   //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{

	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	
	LOG("Destroying Editor");
	for (std::vector<Window*>::iterator it = editorWindows.begin(), end = editorWindows.end(); it != end; ++it)
		delete (*it);
	editorWindows.clear();
	
	console = nullptr;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleEditor::SendEvent(SDL_Event& event)
{
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void ModuleEditor::Log(const char* input)
{
	if (console)
		console->AddLog(input);
}

void ModuleEditor::ProcessFPS(float deltaTime)
{
	monitor->AddFPS(deltaTime);
}

void ModuleEditor::UpdateCameraSettings() 
{
	configuration->UpdateCameraSettings();
}

void ModuleEditor::UpdateWindowSizeSettings()
{
	configuration->UpdateWindowSizeSettings();
}

void ModuleEditor::SelectedModel(Model* model)
{
	properties->SelectPropertiesFromModel(model);
}
