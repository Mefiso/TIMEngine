#include "CMaterial.h"
#include "Application.h"
#include "ImporterMaterial.h"
#include "Leaks.h"

CMaterial::CMaterial(GameObject* _owner) : Component(MATERIAL, _owner)
{
}

CMaterial::CMaterial(GameObject* _owner, const int _UUID) : Component(MATERIAL, _owner, _UUID)
{
}

CMaterial::~CMaterial()
{
	textures.clear();
}

void CMaterial::onSave(rapidjson::Value& config, rapidjson::Document& d) const
{
	ImporterMaterial::Save((CMaterial*)this, ("./Library/Materials/" + filename).c_str());
	rapidjson::Value c(rapidjson::kObjectType);
	c.AddMember("UUID", rapidjson::Value(UUID).Move(), d.GetAllocator());
	c.AddMember("Type", rapidjson::Value(GetType()).Move(), d.GetAllocator());

	rapidjson::Value s;
	s = rapidjson::StringRef(filename.c_str(), filename.size());
	c.AddMember("Filename", s, d.GetAllocator());
	c.AddMember("Filesize", rapidjson::Value(filesize).Move(), d.GetAllocator());

	config.PushBack(c, d.GetAllocator());
}

void CMaterial::onLoad(const rapidjson::Value& config)
{
	filename = config["Filename"].GetString();
	filesize = config["Filesize"].GetInt();
	ImporterMaterial::Load("./Library/Materials/" + filename, this, filesize);
}
