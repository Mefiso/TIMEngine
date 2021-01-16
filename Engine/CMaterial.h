#pragma once
#include "Component.h"
#include <Math/float3.h>
#include <vector>

struct Texture;
class CMaterial : public Component
{
public:
	std::vector<Texture*> textures;				// Vector of textures that are used to render the parent GameObject of this Component

	float3 ambient = float3(0.05);
	float3 diffuse = float3(0.5);
	float shininess = 64.0f;
	float3 specular = float3(0.08f);
	int shininessAlpha = 0;

	std::string filename = "";
	int filesize = 0;

public:
	CMaterial(GameObject* _owner);				// Constructor
	CMaterial(GameObject* _owner, const int _UUID);
	~CMaterial();								// Destructor

	// ------ Serialization -------- //
	void onSave(rapidjson::Value& config, rapidjson::Document& d) const override;
	void onLoad(const rapidjson::Value& config) override;
};