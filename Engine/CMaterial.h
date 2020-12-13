#pragma once
#include "Component.h"
#include "ModuleTexture.h"
#include "assimp/scene.h"

class CMaterial : public Component
{
public:
	std::vector<Texture*> textures;

public:
	CMaterial(GameObject* _owner, const aiMaterial* material, const std::string& path);
	~CMaterial();

	std::vector<Texture*>* LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name, const std::string& path);
};