#include "Mesh.h"
#include "GL/glew.h"

Mesh::Mesh(const aiMesh* mesh)
{
	num_vertices = mesh->mNumVertices;
	num_indices = mesh->mNumFaces * 3;
	LoadVBO(mesh);
	Load
}

void Mesh::LoadVBO(const aiMesh* mesh)
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	unsigned vertex_size = (sizeof(float) * 3 + sizeof(float) * 2);
	unsigned buff_size = vertex_size * num_vertices;
	glBufferData(GL_ARRAY_BUFFER, buff_size, nullptr, GL_STATIC_DRAW);
	
	unsigned int data_offset = 0;
	for (unsigned i = 0; i < num_vertices; ++i)
	{
		glBufferSubData(GL_ARRAY_BUFFER, data_offset, sizeof(float) * 3, &mesh->mVertices[i]);
		glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * 3, sizeof(float) * 2, &mesh->mTextureCoords[i]);
		data_offset += vertex_size;
	}
}
