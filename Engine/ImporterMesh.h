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
		unsigned int LoadVBO(CMesh* _cmesh, unsigned int _numVertices, float* _vertices, float* _normals, float* _uvs)
		{
			unsigned int VBO = 0;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			unsigned vertex_size = (sizeof(float) * 3);
			if (_normals)
				vertex_size += (sizeof(float) * 3);
			if (_uvs)
				vertex_size += (sizeof(float) * 2);
			unsigned buff_size = vertex_size * _numVertices;
			glBufferData(GL_ARRAY_BUFFER, buff_size, nullptr, GL_STATIC_DRAW);

			unsigned int data_offset = 0;
			unsigned int j = 0;
			for (unsigned int i = 0u; i < _numVertices; ++i)
			{
				j = i * 3;
				float vtx[3] = { _vertices[j] , _vertices[j+1], _vertices[j+2] };
				glBufferSubData(GL_ARRAY_BUFFER, data_offset, sizeof(float)*3, &vtx);
				if (_normals)
				{
					float nml[3] = { _normals[j], _normals[j + 1], _normals[j + 2] };
					glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * 3, sizeof(float) * 3, &nml);
				}
				if (_uvs)
				{
					j = i * 2;
					float uv[2] = { _uvs[j], _uvs[j + 1] };
					glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * (_normals?6:3), sizeof(float) * 2, &uv);
				}
				// TODO: colors?
				data_offset += vertex_size;
			}
			return VBO;
		}

		// Loads the Element Buffer Object of this mesh
		unsigned int LoadEBO(unsigned int _numIndices, unsigned int* _indices)
		{
			unsigned int EBO = 0;
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

			unsigned index_size = sizeof(unsigned) * _numIndices;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
			unsigned int* indices = (unsigned int*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

			for (unsigned int i = 0u; i < _numIndices; ++i)
			{
				*(indices++) = _indices[i];
			}
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			return EBO;
		}

		// Loads the Vertex Array Object of this mesh
		unsigned int CreateVAO(unsigned int VBO, unsigned int EBO)
		{
			unsigned int VAO = 0;
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
			return VAO;
		}
	}
};

