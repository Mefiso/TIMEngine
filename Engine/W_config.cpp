#include "W_config.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleTexture.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "Model.h"
#include "Leaks.h"


WConfig::WConfig(std::string name) : Window(name),
brightness(1.0f), width(SCREEN_WIDTH), height(SCREEN_HEIGHT), fullscreen(FULLSCREEN), resizable(RESIZABLE), borderless(false), fulldesktop(false), vsync(VSYNC)
{
	
}

WConfig::~WConfig()
{
}

void WConfig::Draw()
{
	int w, h;
	SDL_GetWindowPosition(App->window->window, &w, &h);
	//ImGui::SetNextWindowPos(ImVec2((float)w+App->window->width, (float)h), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowSize(ImVec2(App->window->width*0.3f, App->window->height*0.9f), ImGuiCond_Once);
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}
	WindowHeader();
	InputHeader();
	RendererHeader();
	TextureHeader();
	CameraHeader();
	
	ImGui::End();

}

void WConfig::WindowHeader()
{
	if (ImGui::CollapsingHeader("Window")) {
		/*if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f"))
			App->window->SetBrightness(brightness);
		if (!fullscreen && !fulldesktop) {

			if (ImGui::SliderInt("Width", &App->window->width, 0, 2560)) {
				App->window->SetWindowSize();
				App->camera->onResize(App->window->width / (float)App->window->height);
			}
			if (ImGui::SliderInt("Height", &App->window->height, 0, 1440)) {
				App->window->SetWindowSize();
				App->camera->onResize(App->window->width / (float)App->window->height);
			}
		}

		SDL_DisplayMode mode;
		SDL_GetWindowDisplayMode(App->window->window, &mode);
		ImGui::TextUnformatted("Refresh rate: ");
		ImGui::SameLine();
		ImGui::TextColored({ 0.0, 1.0, 1.0, 1.0 }, "%d", mode.refresh_rate);
		if (ImGui::Checkbox("VSYNC", &vsync))
			App->window->SetVsync(vsync);
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
				width = App->window->width;
				height = App->window->height;

				SDL_GetWindowDisplayMode(App->window->window, &mode);
				App->window->width = mode.w;
				App->window->height = mode.h;
			}
			else {
				App->window->width = width;
				App->window->height = height;
				//this->UpdateWindowSizeSettings();
			}
			App->camera->onResize(App->window->width / (float)App->window->height);
		}*/
	}
}

void WConfig::InputHeader()
{
	if (ImGui::CollapsingHeader("Input"))
	{
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse pos: <INVALID>");
		ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);

		ImGui::Text("Mouse down:");     
		for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
		{
			if (io.MouseDownDuration[i] >= 0.0f) {
				ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]);
			}
		}
		ImGui::Text("Keys down:");      
		for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
		{
			if (io.KeysDownDuration[i] >= 0.0f) {
				ImGui::SameLine();
				ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]);
			}
		}
	}
}

void WConfig::RendererHeader()
{
	if (ImGui::CollapsingHeader("Renderer"))
	{
		ImGui::Checkbox("Enable Depth testing", &App->renderer->depthTest);
		ImGui::Checkbox("Enable Face culling", &App->renderer->cullFace);

		ImGui::InputFloat4("Background color", &App->renderer->backgroundColor[0]);
		if (ImGui::Button("Default background"))
			App->renderer->backgroundColor = { 0.1f, 0.1f, 0.1f, 0.1f };
		ImGui::Checkbox("Draw grid", &App->renderer->showGrid);
	}
}

// Texture
const char* wrap[] = { "Repeat", "Clamp", "Clamp to border", "Mirrored Repeat" };
const char* filterm[] = { "Linear, Mipmap linear", "Linear, Mipmap nearest", "Nearest, Mipmap linear", "Nearest, Mipmap nearest" };
const char* filterM[] = { "Linear", "Nearest" };
void WConfig::TextureHeader()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		HelpMarker("For this options to be applied reload the model.");
		ImGui::Checkbox("Mipmap", &App->textureLoader->mipmap);
		ImGui::Checkbox("Force flip", &App->textureLoader->force_flip);
		ImGui::Separator();

		if (ImGui::BeginTabBar("Current model textures"))
		{
			std::string label;
			for (unsigned int i = 0; i < App->renderer->modelLoaded->textures.size(); ++i)
			{
				label = "Texture " + std::to_string(i);
				if (ImGui::BeginTabItem(label.c_str()))
				{
					glBindTexture(GL_TEXTURE_2D, App->renderer->modelLoaded->textures[i]->id);

					auto it = std::find(wrapmode.begin(), wrapmode.end(), App->renderer->modelLoaded->textures[i]->wraps);
					int indexWS = it - wrapmode.begin();
					if (ImGui::Combo("Wrap options (S direction)", &indexWS, wrap, IM_ARRAYSIZE(wrap)))
						App->renderer->modelLoaded->textures[i]->wraps = wrapmode[indexWS];
					
					it = std::find(wrapmode.begin(), wrapmode.end(), App->renderer->modelLoaded->textures[i]->wrapt);
					int indexWT = it - wrapmode.begin();
					if (ImGui::Combo("Wrap options (T direction)", &indexWT, wrap, IM_ARRAYSIZE(wrap)))
						App->renderer->modelLoaded->textures[i]->wrapt = wrapmode[indexWT];

					it = std::find(filtermode.begin(), filtermode.end() - 2, App->renderer->modelLoaded->textures[i]->minfilter);
					int indexFm = it - filtermode.begin();
					if (ImGui::Combo("Minification filter", &indexFm, filterm, IM_ARRAYSIZE(filterm)))
						App->renderer->modelLoaded->textures[i]->minfilter = filtermode[indexFm];

					it = std::find(filtermode.end() - 2, filtermode.end(), App->renderer->modelLoaded->textures[i]->magfilter);
					int indexFM = it - (filtermode.end()-2);
					if (ImGui::Combo("Magnification filter", &indexFM, filterM, IM_ARRAYSIZE(filterM)))
						App->renderer->modelLoaded->textures[i]->magfilter = filtermode[4+indexFM];
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
}

void WConfig::CameraHeader()
{
	
	if (ImGui::CollapsingHeader("Camera")) {
		float3 pos = App->camera->frustum.Pos();
		if (ImGui::InputFloat3("Position", &pos[0])) {
			App->camera->frustum.SetPos(pos);
		}
		ImGui::Separator();
		
		ImGui::TextUnformatted("Movement");
		ImGui::InputFloat("Mov Speed", &App->camera->MovementSpeed, 0.1f, 0.5f, "%.2f");
		ImGui::InputFloat("Rot Speed", &App->camera->RotationSpeed, 0.2f, 1.0f, "%.1f");
		ImGui::SliderFloat("Mouse Sens", &App->camera->MouseSensitivity, 0.001f, 0.01f, "%.4f");
		ImGui::Separator();

		ImGui::TextUnformatted("Frustum");
		float nearPlane = App->camera->frustum.NearPlaneDistance();
		if (ImGui::InputFloat("Near Plane", &nearPlane, 0.1f, 1.0f, "%.3f")) {
			App->camera->frustum.SetViewPlaneDistances(nearPlane, App->camera->frustum.FarPlaneDistance());
		}
		float farPlane = App->camera->frustum.FarPlaneDistance();
		if (ImGui::InputFloat("Far Plane", &farPlane, 5.f, 20.0f, "%.3f")) {
			App->camera->frustum.SetViewPlaneDistances(App->camera->frustum.NearPlaneDistance(), farPlane);
		}

		float VFOV = App->camera->frustum.VerticalFov();
		if (ImGui::SliderAngle("FOV", &VFOV)) {
			App->camera->frustum.SetVerticalFovAndAspectRatio(VFOV, App->camera->frustum.AspectRatio());
		}
		float aspectRatio = App->camera->frustum.AspectRatio();
		if (ImGui::InputFloat("Aspect Ratio", &aspectRatio, 0.01f, 0.1f, "%.2f")) {
			App->camera->frustum.SetVerticalFovAndAspectRatio(App->camera->frustum.VerticalFov(), aspectRatio);
		}
		ImGui::SameLine();
		HelpMarker("This alters apect ratio without altering the window size, so it will deform the geometry. Resizeing the window restores geometry.");
	}
	
}
