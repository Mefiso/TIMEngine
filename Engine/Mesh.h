#pragma once
#include "Globals.h"
#include "Math/float2.h"
#include "Math/float3.h"

#include <assimp/scene.h>
#include <vector>

class Mesh
{
public:
	unsigned int VAO;
	unsigned int num_vertices;
	unsigned int num_indices;

	Mesh(const aiMesh* mesh);

private:
	unsigned int VBO, EBO;
	
	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
};