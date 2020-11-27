#pragma once
#include "Globals.h"
#include "Application.h"

#include "Mesh.h"
#include "ModuleTexture.h"
#include <assimp/scene.h>

#include <vector>
#include <string>

class Model
{
public:
	std::vector<Mesh*> meshes;
	// For the first assignment there's only when texture per model, but I put a vector thinking that in the future we'll have to support more
	// In the future, when we have more than one texture per material, we should rethink this, maybe a vector of vector or the meshes contain their textures
	std::vector<Texture*> textures; 
	std::string directory;
	float4x4 transformation = float4x4::identity; // in the future will be updated according to the transformation applied to the GameObject

	Model();
	~Model();

	void Load(std::string const& path);
	void Draw(unsigned int program);
	void ReloadTexture(const char* path);
	
private:
	void CleanUp();
	void LoadMeshes(const aiScene* scene);
	void LoadTextures(const aiScene* scene);

private:
	std::vector<Texture> loaded_textures;

};