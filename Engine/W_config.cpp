#include "W_config.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleTexture.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "GL/glew.h"
#include "Leaks.h"

WConfig::WConfig(std::string name, int ID) : Window(name, ID),
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
	WindowHeader();
	RendererHeader();
	TextureHeader();
	CameraHeader();
	
	ImGui::End();

}

void WConfig::UpdateWindowSizeSettings()
{
	width = App->window->width;
	height = App->window->height; 
}

void WConfig::WindowHeader()
{
	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f"))
			App->window->SetBrightness(brightness);
		if (!fullscreen && !fulldesktop) {

			if (ImGui::SliderInt("Width", &width, 0, 2560)) {
				App->window->SetWindowSize(width, height);
				App->camera->onResize(width / (float)height);
			}
			if (ImGui::SliderInt("Height", &height, 0, 1440)) {
				App->window->SetWindowSize(width, height);
				App->camera->onResize(width / (float)height);
			}
		}

		SDL_DisplayMode mode;
		SDL_GetWindowDisplayMode(App->window->window, &mode);
		ImGui::TextUnformatted("Refresh rate: ");
		ImGui::SameLine();
		ImGui::TextColored({ 0.0, 1.0, 1.0, 1.0 }, "%d", mode.refresh_rate);

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
			App->camera->onResize(width / (float)height);
		}
	}
}

void WConfig::RendererHeader()
{
	if (ImGui::CollapsingHeader("Renderer"))
	{
		ImGui::InputFloat4("Background color", &App->renderer->backgroundColor[0]);
		if (ImGui::Button("Default background"))
			App->renderer->backgroundColor = { 0.1, 0.1, 0.1, 0.1 };
		ImGui::Checkbox("Draw grid", &App->renderer->showGrid);
	}
}

void WConfig::TextureHeader()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		HelpMarker("For this options to be applied reload the model.");
		ImGui::Checkbox("Mipmap", &App->textureLoader->mipmap);
		ImGui::Checkbox("Force flip", &App->textureLoader->force_flip);

		ImGui::TextUnformatted("Wrap options (S direction)");
		ImGui::RadioButton("S Repeat", &App->textureLoader->wrap_s, GL_REPEAT); ImGui::SameLine();
		ImGui::RadioButton("S Clamp", &App->textureLoader->wrap_s, GL_CLAMP); ImGui::SameLine();
		ImGui::RadioButton("S Clamp to border", &App->textureLoader->wrap_s, GL_CLAMP_TO_BORDER); ImGui::SameLine();
		ImGui::RadioButton("S Mirrored repeat", &App->textureLoader->wrap_s, GL_MIRRORED_REPEAT);
		ImGui::TextUnformatted("Wrap options (T direction)");
		ImGui::RadioButton("T Repeat", &App->textureLoader->wrap_t, GL_REPEAT); ImGui::SameLine();
		ImGui::RadioButton("T Clamp", &App->textureLoader->wrap_t, GL_CLAMP); ImGui::SameLine();
		ImGui::RadioButton("T Clamp to border", &App->textureLoader->wrap_t, GL_CLAMP_TO_BORDER); ImGui::SameLine();
		ImGui::RadioButton("T Mirrored repeat", &App->textureLoader->wrap_t, GL_MIRRORED_REPEAT);
		ImGui::Separator();

		ImGui::TextUnformatted("Minification filter");
		if (App->textureLoader->mipmap)
		{
			ImGui::RadioButton("Linear, Mipmap linear", &App->textureLoader->filter_min, GL_LINEAR_MIPMAP_LINEAR); ImGui::SameLine();
			ImGui::RadioButton("Linear, Mipmap nearest", &App->textureLoader->filter_min, GL_LINEAR_MIPMAP_NEAREST);
			ImGui::RadioButton("Nearest, Mipmap linear", &App->textureLoader->filter_min, GL_NEAREST_MIPMAP_LINEAR); ImGui::SameLine();
			ImGui::RadioButton("Nearest, Mipmap nearest", &App->textureLoader->filter_min, GL_NEAREST_MIPMAP_NEAREST);
		}
		else
		{
			ImGui::RadioButton("Min Linear", &App->textureLoader->filter_min, GL_LINEAR); ImGui::SameLine();
			ImGui::RadioButton("Min Nearest", &App->textureLoader->filter_min, GL_NEAREST);
		}
		ImGui::TextUnformatted("Magnification filter");
		ImGui::RadioButton("Mag Linear", &App->textureLoader->filter_mag, GL_LINEAR); ImGui::SameLine();
		ImGui::RadioButton("Mag Nearest", &App->textureLoader->filter_mag, GL_NEAREST);

	}
}

void WConfig::CameraHeader()
{
	if (ImGui::CollapsingHeader("Camera")) {
		if (ImGui::InputFloat3("Position", &App->camera->Position[0])) {
			App->camera->onCameraSettingsChanged();
		}
		ImGui::Separator();
		ImGui::TextUnformatted("Movement");
		if (ImGui::InputFloat("Mov Speed", &App->camera->MovementSpeed, 0.1f, 0.5f, "%.2f")) {
			App->camera->onCameraSettingsChanged();
		}
		if (ImGui::InputFloat("Rot Speed", &App->camera->RotationSpeed, 1.0f, 5.0f, "%.1f")) {
			App->camera->onCameraSettingsChanged();
		}
		if (ImGui::SliderFloat("Mouse Sens", &App->camera->MouseSensitivity, 0.05f, 0.5f, "%.3f")) {
			App->camera->onCameraSettingsChanged();
		}
		ImGui::Separator();

		ImGui::TextUnformatted("Frustum");
		if (ImGui::InputFloat("Near Plane", &App->camera->nearPlane, 0.1f, 1.0f, "%.3f")) {
			App->camera->onCameraSettingsChanged();
		}
		if (ImGui::InputFloat("Far Plane", &App->camera->farPlane, 10.f, 30.0f, "%.3f")) {
			App->camera->onCameraSettingsChanged();
		}

		if (ImGui::SliderAngle("FOV", &App->camera->VFOV)) {
			App->camera->HFOV = App->camera->VFOV * App->camera->aspectRatio;
			App->camera->onCameraSettingsChanged();
		}
		if (ImGui::InputFloat("Aspect Ratio", &App->camera->aspectRatio, 0.01f, 0.1f, "%.2f")) {
			App->camera->HFOV = App->camera->VFOV * App->camera->aspectRatio;
			App->camera->onCameraSettingsChanged();
		}
		ImGui::SameLine();
		HelpMarker("This alters apect ratio without altering the window size, so it will deform the geometry. Resizeing the window restores geometry.");
	}
}
