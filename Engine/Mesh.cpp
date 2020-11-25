#include "Mesh.h"
#include "GL/glew.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"

Mesh::Mesh(const aiMesh* mesh, unsigned int index) : material_index(index)
{
	num_vertices = mesh->mNumVertices;
	num_indices = mesh->mNumFaces * 3;
	LoadVBO(mesh);
	LoadEBO(mesh);
	CreateVAO();
}

void Mesh::Draw(unsigned int program, const std::vector<Texture*>& model_textures, float4x4 model)
{
	App->program->use(program);

	App->program->setMat4(program, "model", model);
	App->program->setMat4(program, "view", App->camera->ViewMatrix());
	App->program->setMat4(program, "proj", App->camera->ProjectionMatrix());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model_textures[material_index]->id);
	App->program->setInt(program, "diffuse", 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);
}

void Mesh::CleanUp()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
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
		glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * 3, sizeof(float) * 2, &mesh->mTextureCoords[0][i]);
		data_offset += vertex_size;
	}
	
}

void Mesh::LoadEBO(const aiMesh* mesh)
{
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	unsigned index_size = sizeof(unsigned) * num_indices;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
	unsigned int* indices = (unsigned int*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3);

		*(indices++) = mesh->mFaces[i].mIndices[0];
		*(indices++) = mesh->mFaces[i].mIndices[1];
		*(indices++) = mesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void Mesh::CreateVAO()
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