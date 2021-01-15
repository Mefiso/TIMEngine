#include "W_viewport.h"
#include "Application.h"
#include "ModuleCamera.h"


#include "ImGuizmo.h"
#include "ModuleSceneManager.h"
#include "CTransform.h"

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


	float3x4 view = App->camera->activeCamera->GetFrustum()->ViewMatrix();
	float4 r0 = view.Row(0);
	float4 r1 = view.Row(1);
	float4 r2 = view.Row(2);
	float viewf[12] = {r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w};

	float4x4 proj = App->camera->activeCamera->GetFrustum()->ProjectionMatrix();
	r0 = proj.Row(0);
	r1 = proj.Row(1);
	r2 = proj.Row(2);
	float4 r3 = proj.Row(3);
	float projf[16] = { r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w, r3.x, r3.y, r3.z, r3.w };

	float4x4 trans = App->sceneMng->GetRoot()->GetChildren()[0]->GetChildren()[1]->GetTransform()->GetTransformationMatrix();
	r0 = trans.Row(0);
	r1 = trans.Row(1);
	r2 = trans.Row(2);
	r3 = trans.Row(3);
	float transf[16] = { r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w, r3.x, r3.y, r3.z, r3.w };
	ImGuizmo::Manipulate(viewf, projf, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, transf);
	
	
	
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