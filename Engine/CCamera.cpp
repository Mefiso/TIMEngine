#include "Application.h"
#include "ModuleRender.h"
#include "CCamera.h"
#include "debugdraw.h"

CCamera::CCamera(GameObject* _owner) : Component(CAMERA, _owner)
{
	frustum->SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum->SetViewPlaneDistances(0.1f, 200.0f);
	frustum->SetVerticalFovAndAspectRatio(DegToRad(60.f), SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	frustum->SetPos(float3(0, 0, 0));
	frustum->SetFront(-float3::unitZ);
	frustum->SetUp(float3(0, 1, 0));
}

CCamera::CCamera(GameObject* _owner, const int _UUID) : Component(CAMERA, _owner, _UUID)
{
	frustum->SetKind(FrustumSpaceGL, FrustumRightHanded);
}

CCamera::~CCamera()
{
	RELEASE(frustum);
}

void CCamera::Draw()
{
	if (owner->isSelected)
		dd::frustum(frustum->ViewProjMatrix().Inverted(), dd::colors::Gray);
}

void CCamera::UpdateFrustumFromTransform(CTransform* _transform)
{
	frustum->SetWorldMatrix(_transform->GetTransformationMatrix().Float3x4Part());
	PerformFrustumCulling(true);
}

void CCamera::UpdateTransformFromFrustum()
{
	if (owner)
	{
		CTransform* tf = owner->GetTransform();
		if (tf)
		{
			tf->SetPos((float3)frustum->Pos());
			tf->SetRotation(frustum->WorldMatrix().ToEulerXYZ());
			tf->UpdateTransformMatrix();
			owner->GetParent()->UpdateBoundingBoxes();
		}
	}
}

void CCamera::PerformFrustumCulling(bool extractFrustum)
{
	if (extractFrustum)
	{
		// Compute frustum planes
		float4x4 viewProjMatrix = frustum->ViewProjMatrix();

		frustumPlanes[0] = viewProjMatrix.Row(3) + viewProjMatrix.Row(0);
		frustumPlanes[1] = viewProjMatrix.Row(3) - viewProjMatrix.Row(0);
		frustumPlanes[2] = viewProjMatrix.Row(3) + viewProjMatrix.Row(1);
		frustumPlanes[3] = viewProjMatrix.Row(3) - viewProjMatrix.Row(1);
		frustumPlanes[4] = viewProjMatrix.Row(3) + viewProjMatrix.Row(2);
		frustumPlanes[5] = viewProjMatrix.Row(3) - viewProjMatrix.Row(2);

		frustum->GetCornerPoints(frustumPoints);
	}

	App->renderer->PerformFrustumCulling(frustumPlanes, frustumPoints);
}

void CCamera::onSave(rapidjson::Value& config, rapidjson::Document& d) const
{
	rapidjson::Value c(rapidjson::kObjectType);
	c.AddMember("UUID", rapidjson::Value(UUID).Move(), d.GetAllocator());
	c.AddMember("Type", rapidjson::Value(GetType()).Move(), d.GetAllocator());

	c.AddMember("NearPlane", rapidjson::Value(frustum->NearPlaneDistance()).Move(), d.GetAllocator());
	c.AddMember("FarPlane", rapidjson::Value(frustum->FarPlaneDistance()).Move(), d.GetAllocator());
	c.AddMember("FOV", rapidjson::Value(frustum->VerticalFov()).Move(), d.GetAllocator());
	c.AddMember("AspectRatio", rapidjson::Value(frustum->AspectRatio()).Move(), d.GetAllocator());

	float3 position = frustum->Pos();
	rapidjson::Value pos(rapidjson::kArrayType);
	pos.PushBack(rapidjson::Value(position[0]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(position[1]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(position[2]).Move(), d.GetAllocator());
	c.AddMember("Position", pos, d.GetAllocator());

	float3 front = frustum->Front();
	rapidjson::Value f(rapidjson::kArrayType);
	f.PushBack(rapidjson::Value(front[0]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(front[1]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(front[2]).Move(), d.GetAllocator());
	c.AddMember("Front", f, d.GetAllocator());

	float3 up = frustum->Up();
	rapidjson::Value u(rapidjson::kArrayType);
	u.PushBack(rapidjson::Value(up[0]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(up[1]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(up[2]).Move(), d.GetAllocator());
	c.AddMember("Up", u, d.GetAllocator());

	config.PushBack(c, d.GetAllocator());
}

void CCamera::onLoad(const rapidjson::Value& config)
{
	frustum->SetViewPlaneDistances(config["NearPlane"].GetFloat(), config["FarPlane"].GetFloat());
	frustum->SetVerticalFovAndAspectRatio(config["FOV"].GetFloat(), config["AspectRatio"].GetFloat());
	frustum->SetPos(float3(config["Position"][0].GetFloat(),
		config["Position"][1].GetFloat(),
		config["Position"][2].GetFloat()));
	frustum->SetFront(float3(config["Front"][0].GetFloat(),
		config["Front"][1].GetFloat(),
		config["Front"][2].GetFloat()));
	frustum->SetUp(float3(config["Up"][0].GetFloat(),
		config["Up"][1].GetFloat(),
		config["Up"][2].GetFloat()));
}
