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
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;

	Model(std::string const& path);
	
private:
	void Load(std::string const& path);
	void LoadMeshes(const aiScene* scene);
	void LoadTextures(const aiScene* scene);

private:
	std::vector<Texture> loaded_textures;

};