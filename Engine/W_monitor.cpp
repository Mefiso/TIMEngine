#include "W_monitor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleTimeManager.h"
#include "GL/glew.h"
#include "IL/il.h"
#include "Leaks.h"

WMonitor::WMonitor(std::string name) : Window(name)
{
	SDL_VERSION(&hardware.compiled);
	hardware.CPU_count = SDL_GetCPUCount();
	hardware.CPU_cache = SDL_GetCPUCacheLineSize();
	hardware.RAM = (float)SDL_GetSystemRAM() / 1024.0f;
}

WMonitor::~WMonitor()
{
	fps_log.clear();
}

void WMonitor::Draw()
{
	int w, h;
	SDL_GetWindowPosition(App->window->window, &w, &h);
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	// ----- RESOURCE USAGE ----- //
	// Update FPS buffer
	fpsNow = 1 / App->timeMng->GetRealTimeDeltaTime();//ImGui::GetIO().Framerate;//
	fps_log.push_back(fpsNow);

	sprintf_s(title, 75, "%.3f ms/frame (%.1f FPS). Averaged FPS %.1f", 1000.0 / fpsNow, fpsNow, App->timeMng->GetFrameCount() / App->timeMng->GetRealTime());
	// Remove the 1st element when we have more than 'histNumElements' values in the FPS vector
	if (fps_log.size() > histNumElements)
		fps_log.erase(fps_log.begin());

	// Plot Hist
	ImGui::PlotHistogram("##Histogram", &fps_log[0], histNumElements, 0, title, 0.0f, 300, ImVec2(400, 50));

	// Input
	DrawInputHeader();

	// Caps
	if (ImGui::CollapsingHeader("Software and Hardware"))
	{
		ShowSoftware();
		ShowHardware();
	}

	ImGui::End();
}

void WMonitor::DrawInputHeader()
{
	if (ImGui::CollapsingHeader("Input"))
	{
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse pos: <INVALID>");
		ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);

		ImGui::Text("Mouse down:");
		for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
		{
			if (io.MouseDownDuration[i] >= 0.0f) {
				ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]);
			}
		}
		ImGui::Text("Keys down:");
		for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
		{
			if (io.KeysDownDuration[i] >= 0.0f) {
				ImGui::SameLine();
				ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]);
			}
		}
	}
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
	for (unsigned int i = 0u; i < hardware.caps.size(); ++i) {
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