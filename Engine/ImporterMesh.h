#pragma once
#include "Globals.h"
#include "CMesh.h"
#include <assimp/mesh.h>
#include <GL/glew.h>
#include <Math/float3.h>
#include <Math/float2.h>

class GameObject;
namespace ImporterMesh
{
	bool Import(const aiMesh* _aimesh, GameObject* _parent);	// Imports a mesh from assimp into a GameObject '_parent'
	unsigned int Save(CMesh* _mesh, const char* _filename);			// Saves the information of a Component Mesh into a file, using our custom file format
	bool Load(const char* _filename, GameObject* _parent, unsigned int _filesize);
	//void LoadCheckers(Material* ourMaterial);

	namespace
	{
		// If vertex is a point outside the current min and max points for AABB, it modifies those min/max points
		void CheckMinMaxPoints(float* vertex, CMesh* _cmesh)
		{
			float3 AABBmax = _cmesh->GetAABBmax();
			float3 AABBmin = _cmesh->GetAABBmin();

			AABBmax.x = vertex[0] > AABBmax.x ? vertex[0] : AABBmax.x;
			AABBmax.y = vertex[1] > AABBmax.y ? vertex[1] : AABBmax.y;
			AABBmax.z = vertex[2] > AABBmax.z ? vertex[2] : AABBmax.z;
			
			AABBmin.x = vertex[0] < AABBmin.x ? vertex[0] : AABBmin.x;
			AABBmin.y = vertex[1] < AABBmin.y ? vertex[1] : AABBmin.y;
			AABBmin.z = vertex[2] < AABBmin.z ? vertex[2] : AABBmin.z;

			_cmesh->SetAABBmax(AABBmax);
			_cmesh->SetAABBmin(AABBmin);
		}

		// Loads the Vertex Buffer Object of this mesh
		unsigned int LoadVBO(CMesh* _cmesh, unsigned int _numVertices, float* _verts, unsigned int _vtxSize)
		{
			unsigned int VBO = 0u;
			unsigned buff_size = sizeof(float) * _vtxSize * _numVertices;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, buff_size, _verts, GL_STATIC_DRAW);
			return VBO;
		}

		// Loads the Element Buffer Object of this mesh
		unsigned int LoadEBO(unsigned int _numIndices, unsigned int* _indices)
		{
			unsigned int EBO = 0u;
			unsigned index_size = sizeof(unsigned) * _numIndices;
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, _indices, GL_STATIC_DRAW);
			return EBO;
		}

		// Loads the Vertex Array Object of this mesh
		unsigned int CreateVAO(unsigned int VBO, unsigned int EBO, unsigned int _vtxSize)
		{
			unsigned int VAO = 0u;
			glGenVertexArrays(1, &VAO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

			// TODO: what if normals or uvs are not present?
			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * _vtxSize, (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * _vtxSize, (void*)(sizeof(float) * 3));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * _vtxSize, (void*)(sizeof(float) * 6));

			glBindVertexArray(0);
			return VAO;
		}
	}
};

