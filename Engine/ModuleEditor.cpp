#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "W_viewport.h"
#include "W_console.h"
#include "W_hierarchy.h"
#include "W_monitor.h"
#include "W_config.h"
#include "W_about.h"
#include "W_properties.h"
#include "Leaks.h"
#include "Brofiler.h"

ModuleEditor::ModuleEditor()
{
	editorWindows.push_back(configuration = new WConfig("Configuration"));
	editorWindows.push_back(console = new WConsole("Console"));
	editorWindows.push_back(hierarchy = new WHierarchy("Hierarchy"));
	editorWindows.push_back(monitor = new WMonitor("Monitoring"));
	editorWindows.push_back(properties = new WProperties("Properties"));
	editorWindows.push_back(viewport = new WViewport("Viewport"));
	editorWindows.push_back(about = new WAbout("About"));

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

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	BROFILER_CATEGORY("PreUpdateEditor", Profiler::Color::Orchid);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	BROFILER_CATEGORY("UpdateEditor", Profiler::Color::Orchid);
	ImGuiID dockspaceID = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	DrawMainMenu();

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
	GameObject* toDeleteGO = hierarchy->GetToDelete();
	if (toDeleteGO) {
		if (toDeleteGO->GetParent())
		{
			toDeleteGO->GetParent()->RemoveChild(toDeleteGO->GetUID());
			App->scene->octree.Erase(toDeleteGO);
		}
		RELEASE(toDeleteGO);
		hierarchy->SetToDelete(nullptr);
	}

	Component* toDeleteCMP = properties->GetToDelete();
	if (toDeleteCMP) {
		if (toDeleteCMP->GetOwner())
			toDeleteCMP->GetOwner()->RemoveComponent(toDeleteCMP->GetUID());
		properties->SetToDelete(nullptr);
	}

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

void ModuleEditor::InspectObject(GameObject* _object)
{
	if (properties)
		properties->SetInspectedObject(_object);
}

const GameObject* ModuleEditor::GetSelectedObject() const
{
	return properties->GetSelectedGO();
}

void ModuleEditor::DrawMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ShowMenuFile();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			ShowMenuGameObject();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Layout"))
		{
			for (std::vector<Window*>::iterator it = editorWindows.begin(), end = editorWindows.end(); it != end; ++it)
			{
				if (ImGui::MenuItem((*it)->GetWindowName(), NULL, (*it)->isEnabled()))
					(*it)->Enable(!(*it)->isEnabled());
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

void ModuleEditor::ShowMenuFile()
{
	ImGui::MenuItem("(demo menu)", NULL, false, false);
	ImGui::MenuItem("Only QUIT option is actually working!", NULL, false, false);
	if (ImGui::MenuItem("New")) {}
	if (ImGui::MenuItem("Open", "Ctrl+O")) {}
	if (ImGui::BeginMenu("Open Recent"))
	{
		ImGui::MenuItem("fish_hat.c");
		ImGui::MenuItem("fish_hat.inl");
		ImGui::MenuItem("fish_hat.h");
		if (ImGui::BeginMenu("More.."))
		{
			ImGui::MenuItem("Hello");
			ImGui::MenuItem("Sailor");
			if (ImGui::BeginMenu("Recurse.."))
			{
				ShowMenuFile();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save", "Ctrl+S")) {}
	if (ImGui::MenuItem("Save As..")) {}

	ImGui::Separator();

	if (ImGui::BeginMenu("Options"))
	{
		static bool enabled = true;
		ImGui::MenuItem("Enabled", "", &enabled);
		ImGui::BeginChild("child", ImVec2(0, 60), true);
		for (int i = 0; i < 10; i++)
			ImGui::Text("Scrolling Text %d", i);
		ImGui::EndChild();
		static float f = 0.5f;
		static int n = 0;
		ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
		ImGui::InputFloat("Input", &f, 0.1f);
		ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Colors"))
	{
		float sz = ImGui::GetTextLineHeight();
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
			ImGui::Dummy(ImVec2(sz, sz));
			ImGui::SameLine();
			ImGui::MenuItem(name);
		}
		ImGui::EndMenu();
	}

	ImGui::Separator();

	if (ImGui::MenuItem("Quit Application", "Alt+F4"))
		should_quit = true;
}

void ModuleEditor::ShowMenuGameObject()
{
	if (ImGui::MenuItem("Create Empty GameObject"))
	{
		App->scene->CreateEmptyGameObject();
	}
}