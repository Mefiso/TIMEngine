#include "CTransform.h"
#include "Math/Quat.h"

CTransform::CTransform(GameObject* _owner) : Component(TRANSFORM, _owner), scale(float3::one), rotation(float3::zero), position(float3::zero)
{
}

CTransform::~CTransform()
{
}

void CTransform::UpdateTransformMatrix()
{
	transformMatrix = float4x4::FromTRS(position,
										Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z),
										scale);
}