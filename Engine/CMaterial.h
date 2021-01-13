#pragma once
#include "Component.h"
#include "ModuleTexture.h"
#include "assimp/scene.h"
#include "Math/float3.h"

class CMaterial : public Component
{
public:
	std::vector<Texture*> textures;															// Vector of textures that are used to render the parent GameObject of this Component

	float3 ambient = float3(0.05);
	float3 diffuse = float3(0.5);
	float shininess = 64.0f;
	float3 specular = float3(0.08f);
	int shininessAlpha = 0;

public:
	CMaterial(GameObject* _owner, const aiMaterial* material, const std::string& path);		// Constructor
	~CMaterial();																			// Destructor

	void LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name, const std::string& path, std::vector<Texture*>* _matTextures);	// Load the textures used to render the parent GameObject of this Component from an aiScene
};