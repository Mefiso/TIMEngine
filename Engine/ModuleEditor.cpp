#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ImGUI/imgui_impl_sdl.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include "W_viewport.h"
#include "W_console.h"
#include "W_monitor.h"
#include "W_config.h"
#include "W_about.h"
#include "W_properties.h"
#include "Leaks.h"

ModuleEditor::ModuleEditor()
{
	editorWindows.push_back(viewport = new WViewport("Viewport"));
	editorWindows.push_back(console = new WConsole("Console"));
	editorWindows.push_back(monitor = new WMonitor("Monitoring window"));
	editorWindows.push_back(configuration = new WConfig("Configuration"));
	editorWindows.push_back(about = new WAbout("About"));
	editorWindows.push_back(properties = new WProperties("Properties"));
}

ModuleEditor::~ModuleEditor()
{
	
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init();

	CreateViewport();

	SelectedModel();
	
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
	ImGuiID dockspaceID = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	CreateMainMenu();

	//CreateViewport();
	//ImGui::ShowDemoWindow();
	for (std::vector<Window*>::iterator it = editorWindows.begin(), end = editorWindows.end(); it != end; ++it)
	{
		if (std::strcmp((*it)->GetWindowName(), "About"))
			ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
		if ((*it)->isEnabled())
			(*it)->Draw();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (should_quit) return UPDATE_STOP;
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
		RELEASE(*it);
	editorWindows.clear();

	viewport = nullptr;
	console = nullptr;
	monitor = nullptr;
	configuration = nullptr;
	properties = nullptr;
	about = nullptr;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleEditor::ReceiveEvent(const Event& event)
{
	switch (event.type)
	{
	case Event::file_dropped:
		SelectedModel();
		break;
	}
}

const bool ModuleEditor::IsViewportHovered() const
{
	return viewport->viewportIsHovered;
}

void ModuleEditor::SendEvent(const SDL_Event& event) const
{
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void ModuleEditor::CreateViewport()
{
	viewport->SetColorbuffer(App->renderer->GetTextureColorbuffer(), App->renderer->GetViewportWidth(), App->renderer->GetViewportHeight());
}

void ModuleEditor::Log(const char* input) const
{
	if (console)
		console->AddLog(input);
}

void ModuleEditor::SelectedModel() const
{
	properties->SelectPropertiesFromModel(App->renderer->modelLoaded);
}


void ModuleEditor::CreateMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::BeginMenu("Windows")) 
			{
				for (std::vector<Window*>::iterator it = editorWindows.begin(), end = editorWindows.end(); it != end; ++it)
				{
					if (ImGui::MenuItem((*it)->GetWindowName(), NULL, (*it)->isEnabled()))
						(*it)->Enable(!(*it)->isEnabled());
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Quit"))
			{
				should_quit = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About"))
		{
			if (ImGui::MenuItem("Visit Github page"))
				ShellExecute(NULL, "open", "https://github.com/Mefiso/TIMEngine", NULL, NULL, SW_SHOWNORMAL);
			if (ImGui::MenuItem("Show about window", NULL, about->isEnabled()))
				about->Enable(!about->isEnabled());
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	
}
