#pragma once
#include "Component.h"
#include "ModuleTexture.h"
#include "assimp/scene.h"

class CMaterial : public Component
{
public:
	std::vector<Texture*> textures;															// Vector of textures that are used to render the parent GameObject of this Component

public:
	CMaterial(GameObject* _owner, const aiMaterial* material, const std::string& path);		// Constructor
	~CMaterial();																			// Destructor

	void LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name, const std::string& path, std::vector<Texture*>* _matTextures);	// Load the textures used to render the parent GameObject of this Component from an aiScene
};