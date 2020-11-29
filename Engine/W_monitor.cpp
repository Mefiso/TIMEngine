#include "W_monitor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "GL/glew.h"
#include "IL/il.h"
#include "Leaks.h"

WMonitor::WMonitor(std::string name, int ID) : Window(name, ID)
{
	SDL_VERSION(&hardware.compiled);
	hardware.CPU_count = SDL_GetCPUCount();
	hardware.CPU_cache = SDL_GetCPUCacheLineSize();
	hardware.RAM = (float)SDL_GetSystemRAM() / 1024.0f;
}

WMonitor::~WMonitor()
{
	fps_log.clear();
	ms_log.clear();
}

void WMonitor::Draw()
{
	int w, h;
	SDL_GetWindowPosition(App->window->window, &w, &h);
	ImGui::SetNextWindowPos(ImVec2(w-App->window->width*0.25, h+App->window->height*0.6), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(App->window->width * 0.25, App->window->height * 0.4), ImGuiCond_Once);
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	sprintf_s(title, 37, "Averaged FPS %.1f\nInstant FPS %.1f", frames / elapsedTime, fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	
	if (ImGui::CollapsingHeader("Software and Hardware"))
	{
		
		ShowSoftware();
		ShowHardware();
	}

	ImGui::End();
}

void WMonitor::AddFPS(float deltaTime)
{
	if (fps_log.size() == log_size) {
		for (int i = 1; i < fps_log.size(); ++i) {
			ms_log[i - 1] = ms_log[i];
			fps_log[i - 1] = fps_log[i];
		}
		ms_log[ms_log.size() - 1] = deltaTime * 1000.0f;
		fps_log[fps_log.size() - 1] = 1 / deltaTime;
	}
	else {
		ms_log.push_back(deltaTime * 1000.0f);
		fps_log.push_back(1 / deltaTime);
	}

	
	++frames;
	elapsedTime += deltaTime;
}

void WMonitor::ShowSoftware() const
{
	ImVec4 color = { 0.1f, 1.0f, 0.9f, 1.0f };
	ImGui::TextUnformatted("Supported OpenGL:");
	ImGui::SameLine();
	ImGui::TextColored(color, "%s", glGetString(GL_VERSION));
	ImGui::TextUnformatted("Glew Verion:");
	ImGui::SameLine();
	ImGui::TextColored(color, "%s", glewGetString(GLEW_VERSION));
	ImGui::TextUnformatted("SDL Version:");
	ImGui::SameLine();
	ImGui::TextColored(color, "%d.%d.%d", hardware.compiled.major, hardware.compiled.minor, hardware.compiled.patch);
	ImGui::TextUnformatted("DevIL Version:");
	ImGui::SameLine();
	ImGui::TextColored(color, "%s", ilGetString(IL_VERSION_NUM));
	ImGui::SameLine(); HelpMarker("More info about libraries used and their versions can be found in the about window.");
	ImGui::Separator();
}

void WMonitor::ShowHardware() const
{
	ImVec4 color = { 0.1f, 1.0f, 0.9f, 1.0f };
	ImGui::TextUnformatted("CPUs:");
	ImGui::SameLine();
	ImGui::TextColored(color, "%d (Cache: %dkb)", hardware.CPU_count, hardware.CPU_cache);
	ImGui::TextUnformatted("System RAM:");
	ImGui::SameLine();
	ImGui::TextColored(color, "%.1fGb", hardware.RAM);
	ImGui::TextUnformatted("Caps:");
	ImGui::SameLine();
	for (int i = 0; i < hardware.caps.size(); ++i) {
		if (hardware.has_caps[i]) {
			ImGui::TextColored(color, "%s,", hardware.caps[i]);
			ImGui::SameLine();
		}
	}
	ImGui::Separator();

	ImGui::TextUnformatted("GPU:");
	ImGui::SameLine();
	ImGui::TextColored(color, (const char*)glGetString(GL_RENDERER));
	ImGui::TextUnformatted("Brand:");
	ImGui::SameLine();
	ImGui::TextColored(color, (const char*)glGetString(GL_VENDOR));
	ImGui::TextUnformatted("VRAM budget:");
	ImGui::SameLine();
	int kb = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &kb);
	ImGui::TextColored(color, "%.1f Mb", (float)kb / 1024.0f);
	ImGui::TextUnformatted("VRAM available:");
	ImGui::SameLine();
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &kb);
	ImGui::TextColored(color, "%.1f Mb", (float)kb / 1024.0f);
}
