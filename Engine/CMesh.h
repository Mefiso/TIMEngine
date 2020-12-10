#pragma once
#include "Component.h"
#include "assimp/scene.h"
#include "Geometry/AABB.h"

class CMesh : public Component
{
public:

	unsigned int VAO = 0u;
	unsigned int numVertices = 0u;
	unsigned int numIndices = 0u;
	//unsigned int materialIndex = 0u;
	AABB boundingBox = AABB(float3::zero, float3::zero);

private:

	unsigned int VBO = 0u, EBO = 0u;
	unsigned int program = 0u;

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