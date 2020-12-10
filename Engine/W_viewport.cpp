#include "W_viewport.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"

WViewport::WViewport(std::string name) : Window(name)
{
}

WViewport::~WViewport()
{
}

void WViewport::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (!ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground ))
	{
		ImGui::PopStyleVar();
		ImGui::End();
		return;
	}
	
	// Set viewport size if Resized
	ImVec2 win_size = ImGui::GetWindowSize();
	if (win_size.x != width || win_size.y != height) {
		width = win_size.x;
		height = win_size.y;
		App->camera->onResize(width / height);
	}

	if (ImGui::IsWindowHovered()) {
		App->renderer->ProcessViewportEvents();
		viewportIsHovered = true;
	}
	else {
		viewportIsHovered = false;
	}
	
	// TODO: why need -20 to vertical size good?
	ImGui::Image((ImTextureID) texid, ImVec2(win_size.x, win_size.y-20), ImVec2(0.f,1.f), ImVec2(1.f,0.f));
	ImGui::PopStyleVar();
	ImGui::End();
}

void WViewport::SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height)
{
	texid = _texid;
	width = (float)_width;
	height = (float)_height;
}
