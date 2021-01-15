#include "W_viewport.h"
#include "Application.h"
#include "ModuleCamera.h"
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
	if (!ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground))
	{
		ImGui::PopStyleVar();
		ImGui::End();
		return;
	}

	// Set viewport size if Resized (considering ImGui headers, tabs and scrollbars)
	ImVec2 win_size = ImGui::GetContentRegionAvail();
	if (win_size.x != width || win_size.y != height) {
		width = win_size.x;
		height = win_size.y;
		App->camera->onResize(width / height);

		// Set the Viewport position offsets (Where in whe screen the viewport is located, considering ImGui headers, tabs and scrollbars)
		viewportPosition = float2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
	}

	if (ImGui::IsWindowHovered()) {
		App->camera->ProcessViewportEvents();
		viewportIsHovered = true;
	}
	else {
		viewportIsHovered = false;
	}

	// TODO: Image is deformed when closing the
	ImGui::Image((ImTextureID)texid, ImVec2(win_size.x, win_size.y), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
	//ImgUi::Overlap
	//lskfdsdlkfjdsñ
	ImGui::PopStyleVar();
	ImGui::End();
}

void WViewport::SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height)
{
	texid = _texid;
	width = (float)_width;
	height = (float)_height;
}