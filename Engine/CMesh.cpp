#include "Globals.h"
#include "CMesh.h"
#include "Application.h"
#include "ModuleSceneManager.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "CMaterial.h"
#include "CLight.h"
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
		/* vvv else -> ModuleProgram::setVec3(program, "lightDir", float3(0.5f, 1.0f, 1.3f));*/
		// TODO: if program == multipleBDRF
		std::map<GameObject*, float> pointLights;	// 8
		std::map<GameObject*, float> spotLights;	// 8
		GameObject* directionalLight = nullptr;		// 1
		SelectClosestLights(pointLights, spotLights, directionalLight);

		// Rebuild the maps into Light* lists
		std::vector<CLight*> points;
		std::vector<CLight*> spots;
		transform(pointLights.begin(), pointLights.end(), back_inserter(points), [](const std::map<GameObject*, float>::value_type& val) {return val.first->GetComponent<CLight>(); });
		transform(spotLights.begin(), spotLights.end(), back_inserter(spots), [](const std::map<GameObject*, float>::value_type& val) {return val.first->GetComponent<CLight>(); });
		// Send the lights to shader
			// Ambient color
		ModuleProgram::setVec3(program, "lights.ambientColor", float3(0.01f));	// TODO: GLOBAL ambient light
			// Directional Light
		if (directionalLight)
		{
			CLight* directional = directionalLight->GetComponent<CLight>();
			ModuleProgram::setInt(program, "lights.hasDirectional", 1);
			ModuleProgram::setVec3(program, "lights.directional.direction", directional->GetDir());
			ModuleProgram::setVec3(program, "lights.directional.color", directional->GetColorRef());
			ModuleProgram::setFloat(program, "lights.directional.intensity", directional->GetIntensityRef());
		}
		else {
			ModuleProgram::setInt(program, "lights.hasDirectional", 0);
		}
			// Point Lights
		ModuleProgram::setInt(program, "lights.num_point", (int)points.size());
		for (unsigned int i = 0; i<points.size(); ++i)
		{
			ModuleProgram::setVec3(program, "lights.pointLights[" + std::to_string(i) + "].position", points[i]->GetPos());
			ModuleProgram::setVec3(program, "lights.pointLights["+std::to_string(i)+"].color", points[i]->GetColorRef());
			ModuleProgram::setFloat(program, "lights.pointLights[" + std::to_string(i) + "].intensity", points[i]->GetIntensityRef());
			ModuleProgram::setFloat(program, "lights.pointLights[" + std::to_string(i) + "].Kc", points[i]->GetKcRef());
			ModuleProgram::setFloat(program, "lights.pointLights[" + std::to_string(i) + "].Kl", points[i]->GetKlRef());
			ModuleProgram::setFloat(program, "lights.pointLights[" + std::to_string(i) + "].Kq", points[i]->GetKqRef());
		}
			// Spot Lights
		ModuleProgram::setInt(program, "lights.num_spot", (int)spots.size());
		for (unsigned int i = 0; i < spots.size(); ++i)
		{
			ModuleProgram::setVec3(program, "lights.spotLights[" + std::to_string(i) + "].position", spots[i]->GetPos());
			ModuleProgram::setVec3(program, "lights.spotLights[" + std::to_string(i) + "].direction", spots[i]->GetDir());
			ModuleProgram::setVec3(program, "lights.spotLights[" + std::to_string(i) + "].color", spots[i]->GetColorRef());
			ModuleProgram::setFloat(program, "lights.spotLights[" + std::to_string(i) + "].intensity", spots[i]->GetIntensityRef());
			ModuleProgram::setFloat(program, "lights.spotLights[" + std::to_string(i) + "].Kc", spots[i]->GetKcRef());
			ModuleProgram::setFloat(program, "lights.spotLights[" + std::to_string(i) + "].Kl", spots[i]->GetKlRef());
			ModuleProgram::setFloat(program, "lights.spotLights[" + std::to_string(i) + "].Kq", spots[i]->GetKqRef());
			ModuleProgram::setFloat(program, "lights.spotLights[" + std::to_string(i) + "].innerAng", DegToRad(spots[i]->GetInnerAngRef()));
			ModuleProgram::setFloat(program, "lights.spotLights[" + std::to_string(i) + "].outerAng", DegToRad(spots[i]->GetOuterAngRef()));
		}

		// Camera
		ModuleProgram::setVec3(program, "cameraPos", App->camera->frustum->Pos());

		// Material
		CMaterial* material = owner->GetComponent<CMaterial>();
		if (material)
		{
			ModuleProgram::setVec3(program, "material.ambient", material->ambient);	// TODO: Get this out
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

void CMesh::SelectClosestLights(std::map<GameObject*, float>& _pointLights, std::map<GameObject*, float>& _spotLights, GameObject*& _directionalLight)
{
	for (std::list<GameObject*>::iterator it = App->sceneMng->lightSources.begin(); it != App->sceneMng->lightSources.end(); ++it)
	{
		if ((*it)->GetComponent<CLight>())
		{
			switch ((*it)->GetComponent<CLight>()->GetLType())
			{
			case 0: // Directional
				_directionalLight = (*it);
				break;
			case 1: // Point
				if (_pointLights.size() < 8)
					_pointLights[(*it)] = this->owner->GetModelMatrix().TranslatePart().Distance((*it)->GetModelMatrix().TranslatePart());
				else
				{
					// Get the pointLight with the higher distance
					float maxDist = 0.f;
					GameObject* maxer;
					for (auto const& x : _pointLights)
						if (x.second > maxDist)
						{
							maxDist = x.second;
							maxer = x.first;
						}

					float distance = this->owner->GetModelMatrix().TranslatePart().Distance((*it)->GetModelMatrix().TranslatePart()); // Get the incoming Light distance to this GO

					if (distance < maxDist) // If the distance is lower, replace pointLight with the incoming GO
					{
						_pointLights.erase(maxer);
						_pointLights[(*it)] = distance;
					}
				}
				break;
			case 2: // Spot
				if (_spotLights.size() < 8)
					_spotLights[(*it)] = this->owner->GetModelMatrix().TranslatePart().Distance((*it)->GetModelMatrix().TranslatePart());
				else
				{
					// Get the pointLight with the higher distance
					float maxDist = 0.f;
					GameObject* maxer;
					for (auto const& x : _spotLights)
						if (x.second > maxDist)
						{
							maxDist = x.second;
							maxer = x.first;
						}

					float distance = this->owner->GetModelMatrix().TranslatePart().Distance((*it)->GetModelMatrix().TranslatePart()); // Get the incoming Light distance to this GO

					if (distance < maxDist) // If the distance is lower, replace pointLight with the incoming GO
					{
						_spotLights.erase(maxer);
						_spotLights[(*it)] = distance;
					}
				}
				break;
			}
		}
	}
}
