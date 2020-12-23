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

void CMesh::Draw()
{
	if (owner->GetTransform())
	{
		// TODO: Preguntar al Carlos.
		ModuleProgram::use(program);

		float4x4 model = owner->GetModelMatrix();
		ModuleProgram::setMat4(program, "model", model);
		ModuleProgram::setMat4(program, "view", App->camera->ViewMatrix());
		ModuleProgram::setMat4(program, "proj", App->camera->ProjectionMatrix());

		// This should be set from other parameters not hardcoded
		// Lighting
		ModuleProgram::setVec3(program, "lightDir", float3(0.5f, 1.0f, 1.3f));
		ModuleProgram::setVec3(program, "lightColor", float3(1.0));

		// This should be set from other parameters not hardcoded
		// Camera
		ModuleProgram::setVec3(program, "cameraPos", App->camera->frustum.Pos());

		ModuleProgram::setVec3(program, "material.ambient", float3(0.05f, 0.05f, 0.05f));
		ModuleProgram::setFloat(program, "material.shininess", 64.0f);
		ModuleProgram::setVec3(program, "material.diffuse", float3(0.5f, 0.5f, 0.5f));

		CMaterial* material = owner->GetComponent<CMaterial>();

		// Bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		if (material)
		{
			for (unsigned int i = 0u; i < material->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);					// Activate proper texture unit
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
				ModuleProgram::setInt(program, ("material." + name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, material->textures[i]->id);
			}
		}
		ModuleProgram::setInt(program, "material.hasDiffuseMap", diffuseNr - 1);
		ModuleProgram::setInt(program, "material.hasSpecularMap", specularNr - 1);
		if (specularNr == 1) {
			ModuleProgram::setVec3(program, "material.specular", float3(0.08f));
			//ModuleProgram::setInt(program, "material.shininessAlpha", 1);
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

	unsigned vertex_size = (sizeof(float) * 8);
	unsigned buff_size = vertex_size * numVertices;
	glBufferData(GL_ARRAY_BUFFER, buff_size, nullptr, GL_STATIC_DRAW);

	unsigned int data_offset = 0;
	for (unsigned i = 0u; i < numVertices; ++i)
	{
		glBufferSubData(GL_ARRAY_BUFFER, data_offset, sizeof(float) * 3, &mesh->mVertices[i]);
		CheckMinMaxPoints(float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * 3, sizeof(float) * 3, &mesh->mNormals[i]);
		if (mesh->mTextureCoords[0] != NULL)
			glBufferSubData(GL_ARRAY_BUFFER, data_offset + sizeof(float) * 6, sizeof(float) * 2, &mesh->mTextureCoords[0][i]);
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	glBindVertexArray(0);
}

void CMesh::CheckMinMaxPoints(float3 vertex)
{
	AABBmax.x = vertex.x > AABBmax.x ? vertex.x : AABBmax.x;
	AABBmax.y = vertex.y > AABBmax.y ? vertex.y : AABBmax.y;
	AABBmax.z = vertex.z > AABBmax.z ? vertex.z : AABBmax.z;

	AABBmin.x = vertex.x < AABBmin.x ? vertex.x : AABBmin.x;
	AABBmin.y = vertex.y < AABBmin.y ? vertex.y : AABBmin.y;
	AABBmin.z = vertex.z < AABBmin.z ? vertex.z : AABBmin.z;

}
