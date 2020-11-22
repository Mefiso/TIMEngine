#include "W_config.h"
#include "Application.h"
#include "ModuleWindow.h"

WConfig::WConfig(std::string name, int ID) : Window(name, ID), Position(float3(0, 1, 7)), nearPlane(0.1f), farPlane(200.0f), 
MovementSpeed(SPEED), RotationSpeed(ROTATION_SPEED), MouseSensitivity(SENSITIVITY), aspectRatio(ASPECTRATIO), VFOV(VERTICALFOV),
brightness(1.0f), width(SCREEN_WIDTH), height(SCREEN_HEIGHT), fullscreen(FULLSCREEN), resizable(RESIZABLE), borderless(false), fulldesktop(false)
{
}

WConfig::~WConfig()
{
}

void WConfig::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(420, 500), ImGuiCond_Once);
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f"))
			App->window->SetBrightness(brightness);
		if (ImGui::SliderInt("Width", &width, 0, 2560)) {
			App->window->SetWindowSize(width, height);
			App->camera->onResize(width / (float)height);
		}
		if (ImGui::SliderInt("Height", &height, 0, 1440)) {
			App->window->SetWindowSize(width, height);
			App->camera->onResize(width / (float)height);
		}
		
		SDL_DisplayMode mode;
		SDL_GetWindowDisplayMode(App->window->window, &mode);
		ImGui::TextUnformatted("Refresh rate: ");
		ImGui::SameLine();
		ImGui::TextColored({0.0, 1.0, 1.0, 1.0}, "%d", mode.refresh_rate);

		if (ImGui::Checkbox("Fullscreen", &fullscreen))
			App->window->SetFullscreen(fullscreen);
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
			App->window->SetResizable(resizable);
		if (ImGui::Checkbox("Borderless", &borderless))
			App->window->SetBorderless(borderless);
		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &fulldesktop)) {
			App->window->SetFulldesktop(fulldesktop); 
			if (fulldesktop) {
				SDL_GetWindowDisplayMode(App->window->window, &mode);
				width = mode.w;
				height = mode.h;
			} 
			else {
				this->UpdateWindowSizeSettings();
			}
			App->camera->onResize(width/(float) height);
		}
	}

	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::InputFloat3("Position", &Position[0]);
		ImGui::Separator();
		ImGui::TextUnformatted("Movement");
		ImGui::InputFloat("Mov Speed", &MovementSpeed, 0.1f, 0.5f, "%.2f");
		ImGui::InputFloat("Rot Speed", &RotationSpeed, 1.0f, 5.0f, "%.1f");
		ImGui::SliderFloat("Mouse Sens", &MouseSensitivity, 0.05f, 0.5f, "%.3f");
		ImGui::Separator();

		ImGui::TextUnformatted("Frustum");
		ImGui::InputFloat("Near Plane", &nearPlane, 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("Far Plane", &farPlane, 10.f, 30.0f, "%.3f");

		ImGui::SliderAngle("FOV", &VFOV);
		ImGui::InputFloat("Aspect Ratio", &aspectRatio, 0.01f, 0.1f, "%.2f");
	}
	if (ImGui::IsWindowFocused())
		UpdateCamera();

	
	ImGui::End();

}

void WConfig::UpdateCameraSettings()
{
	Position = App->camera->Position;
	MovementSpeed = App->camera->MovementSpeed;
	RotationSpeed = App->camera->RotationSpeed;
	MouseSensitivity = App->camera->MouseSensitivity;
	nearPlane = App->camera->nearPlane;
	farPlane = App->camera->farPlane;
	VFOV = App->camera->VFOV;
	aspectRatio = App->camera->aspectRatio;
}

void WConfig::UpdateWindowSizeSettings()
{
	width = App->window->width;
	height = App->window->height; 
}

void WConfig::UpdateCamera() 
{
	App->camera->Position = Position;
	App->camera->MovementSpeed = MovementSpeed;
	App->camera->RotationSpeed = RotationSpeed;
	App->camera->MouseSensitivity = MouseSensitivity;
	App->camera->nearPlane = nearPlane;
	App->camera->farPlane = farPlane;
	App->camera->VFOV = VFOV;
	App->camera->aspectRatio = aspectRatio;
	App->camera->HFOV = App->camera->VFOV * App->camera->aspectRatio;
	App->camera->onCameraSettingsChanged();
}
