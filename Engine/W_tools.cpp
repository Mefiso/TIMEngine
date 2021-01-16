#include "W_tools.h"

WTools::WTools(std::string name) : Window(name)
{
}

WTools::~WTools()
{
}

void WTools::Draw()
{
	// TODO: hide tab bar
	// TODO. force window height
	if (!ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoDecoration))
	{
		ImGui::End();
		return;
	}
	ImGui::RadioButton("Transform", &guizmoState, 0); ImGui::SameLine();
	ImGui::RadioButton("Rotate", &guizmoState, 1); ImGui::SameLine();
	ImGui::RadioButton("Scale", &guizmoState, 2); ImGui::SameLine();
	ImGui::RadioButton("LocalAxis", &guizmoMode, 0); ImGui::SameLine();
	ImGui::RadioButton("WorldAxis", &guizmoMode, 1); ImGui::SameLine();
	ImGui::Indent(ImGui::GetWindowWidth() / 2.3f);
	ImGui::RadioButton("Play", &playPauseStop, 0); ImGui::SameLine();
	ImGui::RadioButton("Pause", &playPauseStop, 1); ImGui::SameLine();
	ImGui::RadioButton("Stop", &playPauseStop, 2); ImGui::SameLine();


	ImGui::End();
}
