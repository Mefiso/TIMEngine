#pragma once
#include "Component.h"
#include <Math/float3.h>

class CMesh : public Component
{
private:
	unsigned int numVertices = 0u;										// Number of non-repeated vertices of the Mesh
	unsigned int numIndices = 0u;										// Total number of vertices of the Mesh (includes repeated vertices of adjacent faces). Each index points to a vertex in 'vertices'
	float* vertices = nullptr;											// The list of vertices
	float* normals = nullptr;											// The list of normals of each vertex
	float* uvs = nullptr;												// The list of texture coordinates of each vertex
	unsigned int* indices = nullptr;									// The list of indices
	unsigned int VAO = 0u, VBO = 0u, EBO = 0u;							// Buffer Object identifiers of the mesh this Component represents
	unsigned int program = 0u;											// Shading program used when rendering this mesh
	float3 AABBmin = float3::inf, AABBmax = -float3::inf;				// Limits of the AABB of this mesh

public:
	CMesh(GameObject* _owner);											// Constructor
	~CMesh();															// Destructor

	void Draw() override;												// Sends the information of this component to Render it

	// ---------- Getters ---------- //
	unsigned int GetNumVertices() const { return numVertices; }
	unsigned int GetNumIndices() const { return numIndices; }
	float* GetVertices() const { return vertices; }
	float* GetNormals() const { return normals; }
	float* GetUVs() const { return uvs; }
	unsigned int* GetIndices() const { return indices; }
	unsigned int GetProgram() const { return program; }
	const unsigned int& GetVBO() const { return VBO; }
	const unsigned int& GetEBO() const { return EBO; }
	const unsigned int& GetVAO() const { return VAO; }
	const float3& GetAABBmax() const { return AABBmax; }
	const float3& GetAABBmin() const { return AABBmin; }

	// ---------- Setters ---------- //
	void SetNumVertices(unsigned int _nV) { numVertices = _nV; }
	void SetNumIndices(unsigned int _nI) { numIndices = _nI; }
	void SetVertices(float* _vtx) { vertices = _vtx; }
	void SetNormals(float* _nm) { normals = _nm; }
	void SetIndices(unsigned int* _ind) { indices = _ind; }
	void SetUVs(float* _uvs) { uvs = _uvs; }
	void SetProgram(unsigned int _program) { program = _program; }		// Sets the shading program used to render this Component
	void SetVBO(unsigned int _VBO) { VBO = _VBO; }
	void SetEBO(unsigned int _EBO) { EBO = _EBO; }
	void SetVAO(unsigned int _VAO) { VAO = _VAO; }
	void SetAABBmax(float3 _AABBmax) { AABBmax = _AABBmax; }
	void SetAABBmin(float3 _AABBmin) { AABBmin = _AABBmin; }
};