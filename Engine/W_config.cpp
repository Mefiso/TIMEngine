#include "W_config.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "Leaks.h"

WConfig::WConfig(std::string name) : Window(name),
brightness(1.0f), resizable(RESIZABLE), borderless(false), fulldesktop(false), vsync(VSYNC)
{
}

WConfig::~WConfig()
{
}

void WConfig::Draw()
{
	int w, h;
	SDL_GetWindowPosition(App->window->window, &w, &h);
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}
	DrawWindowHeader();
	DrawRendererHeader();
	DrawCameraHeader();

	ImGui::End();
}

void WConfig::DrawWindowHeader()
{
	if (ImGui::CollapsingHeader("Window")) {
		SDL_DisplayMode mode;
		SDL_GetWindowDisplayMode(App->window->window, &mode);
		ImGui::TextUnformatted("Refresh rate: ");
		ImGui::SameLine();
		ImGui::TextColored({ 0.0, 1.0, 1.0, 1.0 }, "%d", mode.refresh_rate);

		ImGui::Separator();

		ImGui::PushItemWidth(100.f);
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f"))
			App->window->SetBrightness(brightness);

		if (ImGui::Checkbox("FullScreen", &fulldesktop))
			App->window->ToggleFulldesktop(fulldesktop);
		if (!fulldesktop)
		{
			ImGui::Indent(20.f);
			if (ImGui::Checkbox("Borderless", &borderless))
				App->window->ToggleBorderless(borderless);
			if (ImGui::Checkbox("Resizable", &resizable))
				App->window->ToggleResizable(resizable);
			ImGui::Unindent(20.f);
		}
		if (ImGui::Checkbox("VSYNC", &vsync))
			App->window->SetVsync(vsync);
	}
}

void WConfig::DrawRendererHeader()
{
	if (ImGui::CollapsingHeader("Renderer"))
	{
		ImGui::Checkbox("Enable Depth testing", &App->renderer->depthTest);
		ImGui::Checkbox("Enable Face culling", &App->renderer->cullFace);

		ImGui::Checkbox("Draw grid", &App->renderer->showGrid);
		ImGui::ColorEdit3("Set Grid Color", &App->renderer->gridColor[0], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);

		ImGui::SameLine();
		ImGui::Indent(ImGui::GetWindowWidth() - 70);
		if (ImGui::Button("Reset##1"))
			App->renderer->gridColor = { 1.f, 1.f, 1.f };
		ImGui::Unindent(ImGui::GetWindowWidth() - 70);

		ImGui::ColorEdit3("Set Background Color", &App->renderer->backgroundColor[0], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::Indent(ImGui::GetWindowWidth() - 70);
		if (ImGui::Button("Reset##2"))
			App->renderer->backgroundColor = { 0.1f, 0.1f, 0.1f };
		ImGui::Unindent(ImGui::GetWindowWidth() - 70);
	}
}

void WConfig::DrawCameraHeader()
{
	Frustum* frust = App->camera->defaultCamera->GetFrustum();
	if (ImGui::CollapsingHeader("Engine Camera")) {
		static bool posModified = false;
		float3 pos = frust->Pos();
		ImGui::BulletText("Camera Position:");
		ImGui::PushItemWidth(50.f);
		if (ImGui::DragFloat("x", &pos.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.2f", ImGuiSliderFlags_None)) posModified = true;
		ImGui::SameLine(); if (ImGui::DragFloat("y", &pos.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.2f", ImGuiSliderFlags_None)) posModified = true;
		ImGui::SameLine(); if (ImGui::DragFloat("z", &pos.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.2f", ImGuiSliderFlags_None)) posModified = true;
		if (posModified) {
			frust->SetPos(pos);
			posModified = false;
		}

		ImGui::Separator();

		ImGui::TextUnformatted("Movement Speed");
		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("Using Keys", &App->camera->MovementSpeed, 0.5f, 0.5f, 50.f, "%.2f", ImGuiSliderFlags_None);
		ImGui::DragFloat("Mouse Sensitivity", &App->camera->MouseSensitivity, 0.0001f, 0.001f, 0.01f, "%.3f", ImGuiSliderFlags_None);

		ImGui::Separator();

		ImGui::TextUnformatted("Frustum");
		float nearPlane = frust->NearPlaneDistance();
		float farPlane = frust->FarPlaneDistance();
		if (ImGui::InputFloat("Near Plane", &nearPlane, 0.1f, 1.0f, "%.3f")) {
			frust->SetViewPlaneDistances(nearPlane, farPlane);
		}

		if (ImGui::InputFloat("Far Plane", &farPlane, 5.f, 20.0f, "%.3f")) {
			frust->SetViewPlaneDistances(nearPlane, farPlane);
		}

		float VFOV = frust->VerticalFov();
		float ar = frust->AspectRatio();
		if (ImGui::SliderAngle("FOV", &VFOV, 45.f, 110.f)) {
			frust->SetVerticalFovAndAspectRatio(VFOV, ar);
		}
		if (ImGui::SliderFloat("Aspect Ratio", &ar, 0.05f, 24.f)) {
			frust->SetVerticalFovAndAspectRatio(VFOV, ar);	// The aspect ratio is not stored, so when resizing the viewport/window the user can recover the original aspect ratio
		}
	}
}