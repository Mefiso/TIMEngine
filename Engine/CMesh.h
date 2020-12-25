#pragma once
#include "Component.h"
#include "assimp/scene.h"
#include "Math/float3.h"

class CMesh : public Component
{
public:
	unsigned int numVertices = 0u;										// Number of non-repeated vertices of the Mesh
	unsigned int numIndices = 0u;										// Total number of vertices of the Mesh (includes repeated vertices of adjacent faces)
	float3 AABBmin = float3::inf, AABBmax = -float3::inf;

private:
	unsigned int VAO = 0u, VBO = 0u, EBO = 0u;							// Buffer Object identifiers of the mesh this Component represents
	unsigned int program = 0u;											// Shading program used when rendering this mesh

public:
	CMesh(GameObject* _owner, const aiMesh* mesh);						// Constructor
	~CMesh();															// Destructor

	void Draw() override;												// Sends the information of this component to Render it

	// ---------- Getters ---------- //
	unsigned int GetProgram() const { return program; }

	// ---------- Setters ---------- //
	void SetProgram(unsigned int _program) { program = _program; }		// Sets the shading program used to render this Component

private:
	void LoadVBO(const aiMesh* mesh);									// Loads the Vertex Buffer Object of this mesh
	void LoadEBO(const aiMesh* mesh);									// Loads the Element Buffer Object of this mesh
	void CreateVAO();													// Loads the Vertex Array Object of this mesh
	void CheckMinMaxPoints(float3 vertex);								// If vertex is a point outside the current min and max points for AABB, it modifies those min/max points
};