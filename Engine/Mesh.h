#pragma once
#include "Globals.h"

#include <Math/float4x4.h>
#include <assimp/scene.h>
#include <vector>

#include "ModuleTexture.h"

class Mesh
{
public:

	unsigned int VAO = 0;
	unsigned int num_vertices = 0;
	unsigned int num_indices = 0;
	unsigned int material_index = 0;

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