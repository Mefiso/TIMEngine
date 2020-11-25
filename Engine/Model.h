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
	std::vector<Texture*> textures;
	std::string directory;
	float4x4 transformation = float4x4::identity; // in the future will be updated according to the transformation applied to the GameObject

	Model(std::string const& path);
	~Model();

	void Draw(unsigned int program);
	
private:
	void Load(std::string const& path);
	void LoadMeshes(const aiScene* scene);
	void LoadTextures(const aiScene* scene);

private:
	std::vector<Texture> loaded_textures;

};