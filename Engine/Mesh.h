#pragma once
#include "Globals.h"

#include <Math/float4x4.h>
#include <assimp/scene.h>
#include <vector>

#include "ModuleTexture.h"

class Mesh
{
public:

	unsigned int VAO = 0u;
	unsigned int num_vertices = 0u;
	unsigned int num_indices = 0u;
	unsigned int material_index = 0u;

private:

	unsigned int VBO, EBO;


public:

	Mesh(const aiMesh* mesh, unsigned int index);
	void Draw(unsigned int program, const std::vector<Texture*>& model_textures, float4x4 model);
	void CleanUp();

private:

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();
};