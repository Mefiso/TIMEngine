#include "CLight.h"

CLight::CLight(GameObject* _owner) : Component(LIGHT, _owner)
{
}

CLight::CLight(GameObject* _owner, const int _UUID) : Component(LIGHT, _owner, _UUID)
{
}

CLight::~CLight()
{
}

void CLight::onSave(rapidjson::Value& config, rapidjson::Document& d) const
{
	rapidjson::Value c(rapidjson::kObjectType);
	c.AddMember("UUID", rapidjson::Value(UUID).Move(), d.GetAllocator());
	c.AddMember("Type", rapidjson::Value(GetType()).Move(), d.GetAllocator());

	c.AddMember("LType", rapidjson::Value(lType).Move(), d.GetAllocator());
	rapidjson::Value col(rapidjson::kArrayType);
	col.PushBack(rapidjson::Value(lColor[0]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(lColor[1]).Move(),
		d.GetAllocator()).PushBack(rapidjson::Value(lColor[2]).Move(), d.GetAllocator());
	c.AddMember("Color", col, d.GetAllocator());
	c.AddMember("Intensity", rapidjson::Value(lIntensity).Move(), d.GetAllocator());
	c.AddMember("Kc", rapidjson::Value(Kc).Move(), d.GetAllocator());
	c.AddMember("Kl", rapidjson::Value(Kl).Move(), d.GetAllocator());
	c.AddMember("Kq", rapidjson::Value(Kq).Move(), d.GetAllocator());
	c.AddMember("InnerAng", rapidjson::Value(lInnerAng).Move(), d.GetAllocator());
	c.AddMember("OuterAng", rapidjson::Value(lOuterAng).Move(), d.GetAllocator());

	config.PushBack(c, d.GetAllocator());
}

void CLight::onLoad(const rapidjson::Value& config)
{
	lType = config["LType"].GetInt();
	lColor = float3(config["Color"][0].GetFloat(),
		config["Color"][1].GetFloat(),
		config["Color"][2].GetFloat());
	lIntensity = config["Intensity"].GetFloat();
	Kc = config["Kc"].GetFloat();
	Kl = config["Kl"].GetFloat();
	Kq = config["Kq"].GetFloat();
	lInnerAng = config["InnerAng"].GetFloat();
	lOuterAng = config["OuterAng"].GetFloat();

}
