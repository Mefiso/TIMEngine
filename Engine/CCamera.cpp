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
		}
	}
}
