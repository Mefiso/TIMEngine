#include "W_viewport.h"

WViewport::WViewport(std::string name) : Window(name)
{
}

WViewport::~WViewport()
{
}

void WViewport::Draw()
{
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
	if (!ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::End();
		return;
	}
	ImGui::Image((ImTextureID) texid, ImVec2(width, height), ImVec2(0.f,1.f), ImVec2(1.f,0.f));
	ImGui::End();
	ImGui::PopStyleVar(1);
}

void WViewport::SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height)
{
	texid = _texid;
	width = _width;
	height = _height;
}
