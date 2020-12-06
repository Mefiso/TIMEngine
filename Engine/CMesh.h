#pragma once
#include "Component.h"
#include "assimp/scene.h"
#include "Geometry/AABB.h"

class CMesh : public Component
{
public:

	unsigned int VAO = 0u;
	unsigned int num_vertices = 0u;
	unsigned int num_indices = 0u;
	unsigned int material_index = 0u;
	AABB boundingBox = AABB(float3::zero, float3::zero);

private:

	unsigned int VBO = 0u, EBO = 0u;

public:
	CMesh(GameObject* _owner, const aiMesh* mesh, unsigned int mindex);
	~CMesh();

	void Update() override;

private:

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

};