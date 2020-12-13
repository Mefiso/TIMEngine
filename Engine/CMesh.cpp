#include "CMesh.h"
#include "GL/glew.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "CMaterial.h"

#include <string>

#include "Leaks.h"

CMesh::CMesh(GameObject* _owner, const aiMesh* mesh) : Component(MESH, _owner)
{
	numVertices = mesh->mNumVertices;
	numIndices = mesh->mNumFaces * 3u;
	LoadVBO(mesh);
	LoadEBO(mesh);
	CreateVAO();
}

CMesh::~CMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
}

void CMesh::Update()
{
	if (owner->HasTransform())
	{
		ModuleProgram::use(program);

		float4x4 model = owner->GetModelMatrix();
		ModuleProgram::setMat4(program, "model", model);
		ModuleProgram::setMat4(program, "view", App->camera->ViewMatrix());
		ModuleProgram::setMat4(program, "proj", App->camera->ProjectionMatrix());

		CMaterial* material = owner->GetMaterial();

		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0u; i < material->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);					// activate proper texture unit
			std::string number;
			std::string name = material->textures[i]->type;
			if (name == "diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "specular")
				number = std::to_string(specularNr++);			// transfer unsigned int to stream
			/*else if (name == "normal")
				number = std::to_string(normalNr++);			// transfer unsigned int to stream
			else if (name == "height")
				number = std::to_string(heightNr++);			// transfer unsigned int to stream
			*/
			ModuleProgram::setInt(program, (name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, material->textures[i]->id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, material->textures[i]->wraps);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, material->textures[i]->wrapt);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, material->textures[i]->minfilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, material->textures[i]->magfilter);
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
}

void CMesh::LoadVBO(const aiMesh* mesh)
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	unsigned vertex_size = (sizeof(float) * 3 + sizeof(float) * 2);
	unsigned buff_size = vertex_size * numVertices;
	glBufferData(GL_ARRAY_BUFFER, buff_size, nullptr, GL_STATIC_DRAW);

	unsigned int data_offset = 0;
	for (unsigned i = 0u; i < numVertices; ++i)
	{
		glBufferSubData(GL_ARRAY_BUFFER, data_offset, sizeof(float) * 3, &mesh->mVertices[i]);
		glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * 3, sizeof(float) * 2, &mesh->mTextureCoords[0][i]);
		data_offset += vertex_size;
	}
}

void CMesh::LoadEBO(const aiMesh* mesh)
{
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	unsigned index_size = sizeof(unsigned) * numIndices;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned int* indices = (unsigned int*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (unsigned int i = 0u; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3);

		*(indices++) = mesh->mFaces[i].mIndices[0];
		*(indices++) = mesh->mFaces[i].mIndices[1];
		*(indices++) = mesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void CMesh::CreateVAO()
{
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

	glBindVertexArray(0);
}