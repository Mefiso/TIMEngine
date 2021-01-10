#pragma once
#include "Component.h"
#include <assimp/scene.h>
#include <Math/float3.h>

class CMesh : public Component
{
public:
	unsigned int numVertices = 0u;										// Number of non-repeated vertices of the Mesh
	unsigned int numIndices = 0u;										// Total number of vertices of the Mesh (includes repeated vertices of adjacent faces)

private:
	unsigned int VAO = 0u, VBO = 0u, EBO = 0u;							// Buffer Object identifiers of the mesh this Component represents
	unsigned int program = 0u;											// Shading program used when rendering this mesh
	float3 AABBmin = float3::inf, AABBmax = -float3::inf;				// Limits of the AABB of this mesh

public:
	CMesh(GameObject* _owner);											// Constructor
	~CMesh();															// Destructor

	void Draw() override;												// Sends the information of this component to Render it

	// ---------- Getters ---------- //
	unsigned int GetProgram() const { return program; }
	unsigned int &GetVAO() { return VAO; }
	unsigned int &GetVBO() { return VBO; }
	unsigned int &GetEBO() { return EBO; }
	float3 const &GetAABBmax() { return AABBmax; }
	float3 const &GetAABBmin() { return AABBmin; }

	// ---------- Setters ---------- //
	void SetProgram(unsigned int _program) { program = _program; }		// Sets the shading program used to render this Component
	void SetVBO(unsigned int _VBO) { VBO = _VBO; }
	void SetEBO(unsigned int _EBO) { EBO = _EBO; }
	void SetVAO(unsigned int _VAO) { VAO = _VAO; }
};