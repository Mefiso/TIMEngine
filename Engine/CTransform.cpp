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


void CTransform::onSave(rapidjson::Value& config, rapidjson::Document& d) const
{
	rapidjson::Value c(rapidjson::kObjectType);
	c.AddMember("UUID", rapidjson::Value(UUID).Move(), d.GetAllocator());
	c.AddMember("Type", rapidjson::Value(GetType()).Move(), d.GetAllocator());

	rapidjson::Value pos(rapidjson::kArrayType);
	pos.PushBack(rapidjson::Value(position[0]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(position[1]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(position[2]).Move(), d.GetAllocator());
	c.AddMember("Position", pos, d.GetAllocator());

	rapidjson::Value rot(rapidjson::kArrayType);
	rot.PushBack(rapidjson::Value(rotation[0]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(rotation[1]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(rotation[2]).Move(), d.GetAllocator());
	c.AddMember("Rotation", rot, d.GetAllocator());

	rapidjson::Value sca(rapidjson::kArrayType);
	sca.PushBack(rapidjson::Value(scale[0]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(scale[1]),
		d.GetAllocator()).PushBack(rapidjson::Value(scale[2]), d.GetAllocator());
	c.AddMember("Scale", sca, d.GetAllocator());

	config.PushBack(c, d.GetAllocator());
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
