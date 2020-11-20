#include "W_config.h"
#include "Application.h"

WConfig::WConfig(std::string name, int ID) : Window(name, ID), Position(float3(0, 1, 4)), nearPlane(0.1f), farPlane(200.0f), 
MovementSpeed(SPEED), RotationSpeed(ROTATION_SPEED), MouseSensitivity(SENSITIVITY), aspectRatio(ASPECTRATIO), VFOV(VERTICALFOV)
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

void WConfig::UpdateSettings()
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
