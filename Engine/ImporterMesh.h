#pragma once
#include "CMesh.h"
#include <assimp/scene.h>
#include <GL/glew.h>
#include <Math/float3.h>

class GameObject;
namespace ImporterMesh
{
	void Import(const aiMesh* _aimesh, GameObject* _parent);
	//uint64 Save(const Material* ourMaterial, char** fileBuffer);
	//void Load(const char* fileBuffer, Material* ourMaterial);
	//void LoadCheckers(Material* ourMaterial);

	namespace
	{
		// If vertex is a point outside the current min and max points for AABB, it modifies those min/max points
		void CheckMinMaxPoints(float3 vertex, CMesh* _cmesh)
		{
			float3 AABBmax = _cmesh->GetAABBmax();
			AABBmax.x = vertex.x > AABBmax.x ? vertex.x : AABBmax.x;
			AABBmax.y = vertex.y > AABBmax.y ? vertex.y : AABBmax.y;
			AABBmax.z = vertex.z > AABBmax.z ? vertex.z : AABBmax.z;

			float3 AABBmin = _cmesh->GetAABBmin();
			AABBmin.x = vertex.x < AABBmin.x ? vertex.x : AABBmin.x;
			AABBmin.y = vertex.y < AABBmin.y ? vertex.y : AABBmin.y;
			AABBmin.z = vertex.z < AABBmin.z ? vertex.z : AABBmin.z;
		}

		// Loads the Vertex Buffer Object of this mesh
		void LoadVBO(const aiMesh* _aimesh, unsigned int _numVertices, unsigned int VBO, CMesh* _cmesh)
		{
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			unsigned vertex_size = (sizeof(float) * 8);
			unsigned buff_size = vertex_size * _numVertices;
			glBufferData(GL_ARRAY_BUFFER, buff_size, nullptr, GL_STATIC_DRAW);

			unsigned int data_offset = 0;
			for (unsigned i = 0u; i < _numVertices; ++i)
			{
				glBufferSubData(GL_ARRAY_BUFFER, data_offset, sizeof(float) * 3, &_aimesh->mVertices[i]);
				CheckMinMaxPoints(float3(_aimesh->mVertices[i].x, _aimesh->mVertices[i].y, _aimesh->mVertices[i].z), _cmesh);
				glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * 3, sizeof(float) * 3, &_aimesh->mNormals[i]);
				if (_aimesh->mTextureCoords[0] != NULL)
					glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * 6, sizeof(float) * 2, &_aimesh->mTextureCoords[0][i]);
				data_offset += vertex_size;
			}
		}

		// Loads the Element Buffer Object of this mesh
		void LoadEBO(const aiMesh* _aimesh, unsigned int numIndices, unsigned int EBO)
		{
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

			unsigned index_size = sizeof(unsigned) * numIndices;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
			unsigned int* indices = (unsigned int*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

			for (unsigned int i = 0u; i < _aimesh->mNumFaces; ++i)
			{
				assert(_aimesh->mFaces[i].mNumIndices == 3);

				*(indices++) = _aimesh->mFaces[i].mIndices[0];
				*(indices++) = _aimesh->mFaces[i].mIndices[1];
				*(indices++) = _aimesh->mFaces[i].mIndices[2];
			}

			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		}

		// Loads the Vertex Array Object of this mesh
		void CreateVAO(unsigned int VBO, unsigned int EBO, unsigned int VAO)
		{
			glGenVertexArrays(1, &VAO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

			glBindVertexArray(0);
		}
	}
};

