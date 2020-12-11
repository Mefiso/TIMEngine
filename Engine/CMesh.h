#pragma once
#include "Component.h"
#include "assimp/scene.h"
#include "Geometry/AABB.h"

class CMesh : public Component
{
public:
											
	unsigned int numVertices = 0u;										// Number of non-repeated vertices of the Mesh
	unsigned int numIndices = 0u;										// Total number of vertices of the Mesh (includes repeated vertices of adjacent faces)
	AABB boundingBox = AABB(float3::zero, float3::zero);				// Axis Aligned Bounding Box of the Mesh (Local to the Mesh coordinates)

private:

	unsigned int VAO = 0u, VBO = 0u, EBO = 0u;
	unsigned int program = 0u;											// Shading program used when rendering this mesh

public:
	CMesh(GameObject* _owner, const aiMesh* mesh);
	~CMesh();

	void Update() override;

	void SetProgram(unsigned int _program) { program = _program; }

private:

	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();

};