#include "CMesh.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "CMaterial.h"
#include "ImporterMaterial.h"

#include <string>

#include "Leaks.h"

CMesh::CMesh(GameObject* _owner) : Component(MESH, _owner)
{
	LOG("");
}

CMesh::~CMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(uvs);
	RELEASE_ARRAY(indices);
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
		ModuleProgram::setVec3(program, "cameraPos", App->camera->frustum->Pos());

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
