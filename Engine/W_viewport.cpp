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
	if (App->sceneMng->selectedObject)
		DrawGuizmo(App->camera->activeCamera, App->sceneMng->selectedObject);
	
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

void WViewport::DrawGuizmo(CCamera* camera, GameObject* go)
{
	//ComponentCamera* go_camera = static_cast<ComponentCamera*>(go->FindFirstComponent(Component::Camera));

	float4x4 view = camera->GetFrustum()->ViewMatrix();
	float4x4 proj = camera->GetFrustum()->ProjectionMatrix();
	view.Transpose(); proj.Transpose();

	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);

	float4x4 model = go->GetTransform()->GetTransformationMatrix();
	model.Transpose();

	float4x4 delta;

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, width, height);
	ImGuizmo::SetDrawlist();
	ImGuizmo::Manipulate((const float*)&view, (const float*)&proj, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&model, (float*)&delta/*, guizmo_useSnap ? &guizmo_snap.x : NULL*/);

	if (ImGuizmo::IsUsing() && !delta.IsIdentity())
	{
		/*model.Transpose();
		if (guizmo_mode == ImGuizmo::LOCAL || go->GetParent() == nullptr)
		{
			go->SetTransform(model);
		}
		else
		{
			float4x4 parent = go->GetParent()->GetGlobalTransformation();
			parent.InverseOrthonormal();
			go->SetLocalTransform(parent * model);
		}

		App->level->GetRoot()->RecursiveCalcGlobalTransform(float4x4::identity, false);*/

		/*if (go_camera)
		{
			go_camera->OnUpdateTransform();
		}*/
	}


	/*float3 points[8];
	go->global_bbox.GetCornerPoints(points);
	std::swap(points[2], points[5]);
	std::swap(points[3], points[4]);
	std::swap(points[4], points[5]);
	std::swap(points[6], points[7]);
	dd::box(points, dd::colors::Yellow);*/

	/*if (go_camera)
	{
		float4x4 go_view = go_camera->GetViewMatrix();
		float4x4 go_proj = go_camera->GetProjectionMatrix();

		float4x4 inv_clip = go_proj * go_view;
		inv_clip.Inverse();

		dd::frustum(inv_clip, dd::colors::Gray);
	}*/
}