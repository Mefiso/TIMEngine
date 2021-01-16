#include "CMesh.h"
#include "Application.h"
#include "ModuleSceneManager.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "CMaterial.h"
#include "ImporterMaterial.h"
#include "ImporterMesh.h"

#include <string>

#include "Leaks.h"

CMesh::CMesh(GameObject* _owner) : Component(MESH, _owner)
{
}

CMesh::CMesh(GameObject* _owner, const int _UUID) : Component(MESH, _owner, _UUID)
{
}

CMesh::~CMesh()
{
	App->sceneMng->octree.Erase(owner);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	RELEASE_ARRAY(vertices);
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

		// Camera
		ModuleProgram::setVec3(program, "cameraPos", App->camera->frustum->Pos());

		// Material
		CMaterial* material = owner->GetComponent<CMaterial>();
		if (material)
		{
			ModuleProgram::setVec3(program, "material.ambient", material->ambient);
			ModuleProgram::setFloat(program, "material.shininess", material->shininess);
			ModuleProgram::setVec3(program, "material.diffuse", material->diffuse);

			// Bind appropriate textures
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;

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

			// If no diffuse/specular maps
			ModuleProgram::setInt(program, "material.hasDiffuseMap", diffuseNr - 1);
			ModuleProgram::setInt(program, "material.hasSpecularMap", specularNr - 1);
			if (specularNr == 1) {
				ModuleProgram::setVec3(program, "material.specular", material->specular);
				ModuleProgram::setInt(program, "material.shininessAlpha", material->shininessAlpha);
			}
		}
		else
		{
			ModuleProgram::setVec3(program, "material.ambient", float3(0.05));
			ModuleProgram::setFloat(program, "material.shininess", 64.f);
			ModuleProgram::setVec3(program, "material.diffuse", float3(0.5));
			ModuleProgram::setInt(program, "material.hasDiffuseMap", 0);
			ModuleProgram::setInt(program, "material.hasSpecularMap", 0);
			ModuleProgram::setVec3(program, "material.specular", float3(0.08f));
			ModuleProgram::setInt(program, "material.shininessAlpha", 0);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
}

void CMesh::onSave(rapidjson::Value& config, rapidjson::Document& d) const
{
	rapidjson::Value c(rapidjson::kObjectType);
	c.AddMember("UUID", rapidjson::Value(UUID).Move(), d.GetAllocator());
	c.AddMember("Type", rapidjson::Value(GetType()).Move(), d.GetAllocator());

	rapidjson::Value s;
	s = rapidjson::StringRef(filename.c_str(), filename.size());
	c.AddMember("Filename", s, d.GetAllocator());
	c.AddMember("Filesize", rapidjson::Value(filesize).Move(), d.GetAllocator());

	config.PushBack(c, d.GetAllocator());
}

void CMesh::onLoad(const rapidjson::Value& config)
{
	filename = config["Filename"].GetString();
	filesize = config["Filesize"].GetInt();
	ImporterMesh::Load(("./Library/Meshes/"+filename).c_str(), owner, filesize);
}
