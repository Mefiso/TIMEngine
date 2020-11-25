#pragma once
#include "Globals.h"
#include "Math/float2.h"
#include "Math/float3.h"

#include <assimp/scene.h>
#include <vector>

#include "ModuleTexture.h"

class Mesh
{
public:
	unsigned int VAO;
	unsigned int num_vertices;
	unsigned int num_indices;
	unsigned int material_index;
public:
	Mesh(const aiMesh* mesh, unsigned int index);
	void Draw(unsigned int program, const std::vector<Texture*>& model_textures);
	void CleanUp();

private:
	unsigned int VBO, EBO;
private:
	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();
};