#include "W_viewport.h"

WViewport::WViewport(std::string name) : Window(name)
{
}

WViewport::~WViewport()
{
}

void WViewport::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_Once);
	if (!ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
	{
		ImGui::End();
		return;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	// Set viewport size if Resized
	ImVec2 win_size = ImGui::GetContentRegionAvail();
	if (win_size.x != width || win_size.y != height) {
		width = win_size.x;
		height = win_size.y;
		viewportResized = true;
	}

	if (ImGui::IsWindowHovered()) {
		viewportIsHovered = true;
	}
	
	ImGui::Image((ImTextureID) texid, win_size, ImVec2(0.f,1.f), ImVec2(1.f,0.f));
	ImGui::PopStyleVar();
	ImGui::End();
	


}

void WViewport::SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height)
{
	texid = _texid;
	width = _width;
	height = _height;
}
