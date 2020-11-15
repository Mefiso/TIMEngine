#include "W_monitor.h"
#include "ImGUI/imgui.h"

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
	ImGui::SetNextWindowSize(ImVec2(450, 450), ImGuiCond_Once);
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	sprintf_s(title, 37, "Averaged FPS %.1f\nInstant FPS %.1f", frames / elapsedTime, fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	
	if (ImGui::CollapsingHeader("Hardware"))
	{
		ImVec4 color = {0.1f, 1.0f, 0.9f, 1.0f};
		ImGui::TextUnformatted("SDL Version:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%d.%d.%d", hardware.compiled.major, hardware.compiled.minor, hardware.compiled.patch);
		ImGui::Separator();

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
