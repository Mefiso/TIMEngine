#include "W_viewport.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleSceneManager.h"
#include "ImGuizmo.h"


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

	ImGui::Image((ImTextureID)texid, ImVec2(win_size.x, win_size.y), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
	if (App->sceneMng->GetSelectedGO())
		if (App->sceneMng->GetSelectedGO()->GetTransform())
			DrawGuizmo(App->camera->activeCamera, App->sceneMng->GetSelectedGO());

	ImGui::PopStyleVar();
	ImGui::End();
}

void WViewport::SetColorbuffer(unsigned int _texid, unsigned int _width, unsigned int _height)
{
	texid = _texid;
	width = (float)_width;
	height = (float)_height;
}

void WViewport::DrawGuizmo(CCamera* _camera, GameObject* _go)
{
	ImGuizmo::OPERATION ops;
	switch (App->editor->GetGuizmoState())
	{
	case 1:
		ops = ImGuizmo::ROTATE;
		break;
	case 2:
		ops = ImGuizmo::SCALE;
		break;
	case 0:
	default:
		ops = ImGuizmo::TRANSLATE;
		break;
	}

	ImGuizmo::MODE mode;
	switch (App->editor->GetGuizmoMode())
	{
	case 1:
		mode = ImGuizmo::WORLD;
		break;
	case 0:
	default:
		mode = ImGuizmo::LOCAL;
		break;
	}

	float4x4 view = _camera->GetFrustum()->ViewMatrix();
	float4x4 proj = _camera->GetFrustum()->ProjectionMatrix();
	float4x4 model = _go->GetModelMatrix();
	float4x4 delta;
	view.Transpose(); proj.Transpose(); model.Transpose();

	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, width, height);
	ImGuizmo::SetDrawlist();

	
	ImGuizmo::Manipulate((const float*)&view, (const float*)&proj, ops, mode, (float*)&model, (float*)&delta/*, guizmo_useSnap ? &guizmo_snap.x : NULL*/);

	if (ImGuizmo::IsUsing() && !delta.IsIdentity())
	{
		// set the new transform
		model.Transpose();
		// TODO: Set the transform correctly (when rotating/scaling)
		/*Maybe:
		float3 thisScale = float3(thisTransform.Col3(0).Length(), thisTransform.Col3(1).Length(), thisTransform.Col3(2).Length());
		if (thisScale.Equals(float3::one))
			thisTransform.InverseOrthonormal();
		else if (thisScale.xxx().Equals(thisScale))
			thisTransform.InverseOrthogonalUniformScale();
		else
			thisTransform.InverseColOrthogonal();*/
		float4x4 parent = _go->GetParent()->GetModelMatrix().Inverted();
		_go->SetTransform(parent * model);
		_go->UpdateBoundingBoxes();
		_go->UpdateOctreePosition();
		
		// set the GO camera frustum transform, if exists this component
		if (App->sceneMng->GetSelectedGO()->GetComponent<CCamera>())
			App->sceneMng->GetSelectedGO()->GetComponent<CCamera>()->UpdateFrustumFromTransform(_go->GetTransform());
	}
}