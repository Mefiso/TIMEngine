#include "W_config.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleTexture.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "Model.h"
#include "Leaks.h"


WConfig::WConfig(std::string name) : Window(name),
brightness(1.0f), fullscreen(FULLSCREEN), resizable(RESIZABLE), borderless(false), fulldesktop(false), vsync(VSYNC)
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
		SDL_DisplayMode mode;
		SDL_GetWindowDisplayMode(App->window->window, &mode);
		ImGui::TextUnformatted("Refresh rate: ");
		ImGui::SameLine();
		ImGui::TextColored({ 0.0, 1.0, 1.0, 1.0 }, "%d", mode.refresh_rate);

		ImGui::Separator();

		ImGui::PushItemWidth(100.f);
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f"))
			App->window->SetBrightness(brightness);
		// TODO ? : Screen Resize
		/*if (!fullscreen && !fulldesktop) {
			//ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.20f);
			ImGui::TextUnformatted("Window Size:");
			ImGui::SameLine(); 
			static int winWidth = App->window->GetWindowWidth();
			static int winHeight = App->window->GetWindowHeight();
			if (ImGui::DragFloat("Width", &winWidth, 0.005f, +FLT_MAX, +FLT_MAX, "%.2f", ImGuiSliderFlags_None)) {
				App->window->SetWindowSize();
				App->camera->onResize(winWidth / (float)App->window->GetWindowHeight());
			}
			ImGui::SameLine();
			
			if (ImGui::DragFloat("Width", &winHeight, 0.005f, +FLT_MAX, +FLT_MAX, "%.2f", ImGuiSliderFlags_None)) {
				App->window->SetWindowSize();
				App->camera->onResize(App->window->GetWindowWidth() / winHeight);
			}
			App->window->SetWindowSize();
			App->camera->onResize(winWidth / winHeight);*/

			/*if (ImGui::SliderInt("Width", &winWidth, 15, 2560)) {
				Event ev(Event::window_resize);
				ev.point2d.x = winWidth;
				ev.point2d.y = winHeight;
				App->BroadcastEvent(ev);
			}*/
			/*if (ImGui::SliderInt("Width", App->window->GetWindowWidthPtr(), 0, 1536)) {
				
				App->camera->onResize(App->window->GetWindowWidth() / (float)App->window->GetWindowHeight());
				App->window->SetWindowSize();
			}
			if (ImGui::SliderInt("Height", &winHeight, 15, 809)) {
				App->window->SetWindowSize();
				App->camera->onResize(App->window->GetWindowWidth() / (float)winHeight);
			}
		}*/

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
				ImGui::PushItemWidth(100);
				if (ImGui::BeginTabItem(label.c_str()))
				{
					glBindTexture(GL_TEXTURE_2D, App->renderer->modelLoaded->textures[i]->id);

					auto it = std::find(wrapmode.begin(), wrapmode.end(), App->renderer->modelLoaded->textures[i]->wraps);
					int indexWS = it - wrapmode.begin();
					if (ImGui::Combo("Wrap (S dir)", &indexWS, wrap, IM_ARRAYSIZE(wrap)))
						App->renderer->modelLoaded->textures[i]->wraps = wrapmode[indexWS];
					
					it = std::find(wrapmode.begin(), wrapmode.end(), App->renderer->modelLoaded->textures[i]->wrapt);
					int indexWT = it - wrapmode.begin();
					if (ImGui::Combo("Wrap (T dir)", &indexWT, wrap, IM_ARRAYSIZE(wrap)))
						App->renderer->modelLoaded->textures[i]->wrapt = wrapmode[indexWT];

					it = std::find(filtermode.begin(), filtermode.end() - 2, App->renderer->modelLoaded->textures[i]->minfilter);
					int indexFm = it - filtermode.begin();
					if (ImGui::Combo("Minification", &indexFm, filterm, IM_ARRAYSIZE(filterm)))
						App->renderer->modelLoaded->textures[i]->minfilter = filtermode[indexFm];

					it = std::find(filtermode.end() - 2, filtermode.end(), App->renderer->modelLoaded->textures[i]->magfilter);
					int indexFM = it - (filtermode.end()-2);
					if (ImGui::Combo("Magnification", &indexFM, filterM, IM_ARRAYSIZE(filterM)))
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
		static bool posModified = false;
		static float3 pos = App->camera->frustum.Pos();
		ImGui::BulletText("Camera Position:");
		ImGui::PushItemWidth(50.f);
		if (ImGui::DragFloat("x", &pos.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.2f", ImGuiSliderFlags_None)) posModified = true;
		ImGui::SameLine(); if (ImGui::DragFloat("y", &pos.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.2f", ImGuiSliderFlags_None)) posModified = true;
		ImGui::SameLine(); if (ImGui::DragFloat("z", &pos.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.2f", ImGuiSliderFlags_None)) posModified = true;
		if (posModified) {
			App->camera->frustum.SetPos(pos);
			posModified = false;
		}

		ImGui::Separator();
		
		ImGui::TextUnformatted("Movement Speed");
		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("Using Keys", &App->camera->MovementSpeed, 0.5f, 0.5f, 50.f, "%.2f", ImGuiSliderFlags_None);
		ImGui::DragFloat("Mouse Sensitivity", &App->camera->MouseSensitivity, 0.0001f, 0.001f, 0.01f, "%.3f", ImGuiSliderFlags_None);

		ImGui::Separator();

		ImGui::TextUnformatted("Frustum");
		float nearPlane = App->camera->frustum.NearPlaneDistance();
		float farPlane = App->camera->frustum.FarPlaneDistance();
		if (ImGui::InputFloat("Near Plane", &nearPlane, 0.1f, 1.0f, "%.3f")) {
			App->camera->frustum.SetViewPlaneDistances(nearPlane, App->camera->frustum.FarPlaneDistance());
		}
		
		if (ImGui::InputFloat("Far Plane", &farPlane, 5.f, 20.0f, "%.3f")) {
			App->camera->frustum.SetViewPlaneDistances(App->camera->frustum.NearPlaneDistance(), farPlane);
		}

		float VFOV = App->camera->frustum.VerticalFov();
		if (ImGui::SliderAngle("FOV", &VFOV, 45.f, 110.f)) {
			App->camera->frustum.SetVerticalFovAndAspectRatio(VFOV, App->camera->frustum.AspectRatio());
		}
	}
	
}
