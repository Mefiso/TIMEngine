#include "CLight.h"
#include "ModuleSceneManager.h"

CLight::CLight(GameObject* _owner) : Component(LIGHT, _owner)
{
	SetPositionDirection(_owner->GetModelMatrix());
	App->sceneMng->lightSources.push_back(owner);
}

CLight::CLight(GameObject* _owner, const int _UUID) : Component(LIGHT, _owner, _UUID)
{
	SetPositionDirection(_owner->GetModelMatrix());
	App->sceneMng->lightSources.push_back(owner);
}

CLight::~CLight()
{
	for (std::list<GameObject*>::iterator it = App->sceneMng->lightSources.begin(); it != App->sceneMng->lightSources.end(); ++it)
	{
		if (owner == (*it))
		{
			App->sceneMng->lightSources.erase(it);
			break;
		}
	}
}

void CLight::SetPositionDirection(float4x4 _modelMatrix)
{
	dir = _modelMatrix.RotatePart().Col3(2).Normalized();
	pos = _modelMatrix.TranslatePart();
}

void CLight::onSave(rapidjson::Value& _config, rapidjson::Document& _d) const
{
	rapidjson::Value c(rapidjson::kObjectType);
	c.AddMember("UUID", rapidjson::Value(UUID).Move(), _d.GetAllocator());
	c.AddMember("Type", rapidjson::Value(GetType()).Move(), _d.GetAllocator());

	c.AddMember("LType", rapidjson::Value(lType).Move(), _d.GetAllocator());
	rapidjson::Value col(rapidjson::kArrayType);
	col.PushBack(rapidjson::Value(lColor[0]).Move(),
		_d.GetAllocator()).PushBack(rapidjson::Value(lColor[1]).Move(),
		_d.GetAllocator()).PushBack(rapidjson::Value(lColor[2]).Move(), _d.GetAllocator());
	c.AddMember("Color", col, _d.GetAllocator());
	c.AddMember("Intensity", rapidjson::Value(lIntensity).Move(), _d.GetAllocator());
	c.AddMember("Kc", rapidjson::Value(Kc).Move(), _d.GetAllocator());
	c.AddMember("Kl", rapidjson::Value(Kl).Move(), _d.GetAllocator());
	c.AddMember("Kq", rapidjson::Value(Kq).Move(), _d.GetAllocator());
	c.AddMember("InnerAng", rapidjson::Value(lInnerAng).Move(), _d.GetAllocator());
	c.AddMember("OuterAng", rapidjson::Value(lOuterAng).Move(), _d.GetAllocator());
	rapidjson::Value position(rapidjson::kArrayType);
	position.PushBack(rapidjson::Value(pos[0]).Move(),
		_d.GetAllocator()).PushBack(rapidjson::Value(pos[1]).Move(),
			_d.GetAllocator()).PushBack(rapidjson::Value(pos[2]).Move(), _d.GetAllocator());
	c.AddMember("Position", position, _d.GetAllocator());
	rapidjson::Value direction(rapidjson::kArrayType);
	direction.PushBack(rapidjson::Value(dir[0]).Move(),
		_d.GetAllocator()).PushBack(rapidjson::Value(dir[1]).Move(),
			_d.GetAllocator()).PushBack(rapidjson::Value(dir[2]).Move(), _d.GetAllocator());
	c.AddMember("Direction", direction, _d.GetAllocator());


	_config.PushBack(c, _d.GetAllocator());
}

void CLight::onLoad(const rapidjson::Value& _config)
{
	lType = _config["LType"].GetInt();
	lColor = float3(_config["Color"][0].GetFloat(),
		_config["Color"][1].GetFloat(),
		_config["Color"][2].GetFloat());
	lIntensity = _config["Intensity"].GetFloat();
	Kc = _config["Kc"].GetFloat();
	Kl = _config["Kl"].GetFloat();
	Kq = _config["Kq"].GetFloat();
	lInnerAng = _config["InnerAng"].GetFloat();
	lOuterAng = _config["OuterAng"].GetFloat();
	pos = float3(_config["Position"][0].GetFloat(),
		_config["Position"][1].GetFloat(),
		_config["Position"][2].GetFloat());
	dir = float3(_config["Direction"][0].GetFloat(),
		_config["Direction"][1].GetFloat(),
		_config["Direction"][2].GetFloat());
}
