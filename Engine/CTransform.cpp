#include "CTransform.h"
#include "Math/Quat.h"

CTransform::CTransform(GameObject* _owner) : Component(TRANSFORM, _owner), scale(float3::one), rotation(float3::zero), position(float3::zero)
{
}

CTransform::CTransform(GameObject* _owner, const int _UUID) : Component(TRANSFORM, _owner, _UUID)
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

void CTransform::onSave(rapidjson::Value& config) const
{
}

void CTransform::onLoad(const rapidjson::Value& config)
{
	position = float3(config["Position"][0].GetFloat(),
		config["Position"][1].GetFloat(),
		config["Position"][2].GetFloat());
	rotation = float3(config["Rotation"][0].GetFloat(),
		config["Rotation"][1].GetFloat(),
		config["Rotation"][2].GetFloat());
	scale = float3(config["Scale"][0].GetFloat(),
		config["Scale"][1].GetFloat(),
		config["Scale"][2].GetFloat());

	UpdateTransformMatrix();
}
