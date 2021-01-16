#include "ModuleSceneManager.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "CMesh.h"
#include <Geometry/LineSegment.h>
#include <Geometry/Triangle.h>

ModuleSceneManager::ModuleSceneManager()
{
	root->ChangeName("Scene 1");
}

ModuleSceneManager::~ModuleSceneManager()
{
	CleanUp();
}

bool ModuleSceneManager::Start()
{
	octree.SetBoundaries(AABB(float3(-20, -20, -20), float3(20, 20, 20)));
	InitSkyboxVAO();
	vanillaProgram = ModuleProgram::CreateProgramFromFile(".\\resources\\shaders\\vanilla.vs.glsl", ".\\resources\\shaders\\vanilla.fs.glsl");
	phongProgram = ModuleProgram::CreateProgramFromFile(".\\resources\\shaders\\phong.vs.glsl", ".\\resources\\shaders\\phong.fs.glsl");
	pbrProgram = ModuleProgram::CreateProgramFromFile(".\\resources\\shaders\\bdrfPhong.vs.glsl", ".\\resources\\shaders\\bdrfPhong.fs.glsl");
	defaultProgram = pbrProgram;
	skyboxShader = ModuleProgram::CreateProgramFromFile(".\\resources\\shaders\\skybox.vs.glsl", ".\\resources\\shaders\\skybox.fs.glsl");
	return true;
}

bool ModuleSceneManager::CleanUp()
{
	glDeleteProgram(vanillaProgram);
	glDeleteProgram(phongProgram);
	glDeleteProgram(pbrProgram);
	glDeleteProgram(skyboxShader);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteTextures(1, &skyboxTexture);

	RELEASE(root);

	return true;
}

void ModuleSceneManager::CreateEmptyGameObject()
{
	GameObject* newEmpty = new GameObject("Empty GameObj");
	newEmpty->SetParent(root);
}

void ModuleSceneManager::InitSkyboxVAO()
{
	float skyboxVertices[108] = {
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void ModuleSceneManager::DrawSkybox()
{
	ModuleProgram::use(skyboxShader);
	ModuleProgram::setMat4(skyboxShader, "view", App->camera->ViewMatrix());
	ModuleProgram::setMat4(skyboxShader, "proj", App->camera->ProjectionMatrix());
	ModuleProgram::setInt(skyboxShader, "skybox", 0);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleSceneManager::MousePicker(int _x, int _y)
{
	// deselect all gameobjects
	//selectedObject = nullptr;

	//Normalise x,y to [-1,1]
	float vpX = (_x - App->editor->GetViewportPos().x - App->editor->GetViewportSize().x / 2) / (App->editor->GetViewportSize().x / 2);
	float vpY = (_y - App->editor->GetViewportPos().y - App->editor->GetViewportSize().y / 2) / (App->editor->GetViewportSize().y / 2) * -1; // -1 inverts the Y coordinate (origin of frustum is at LOWERLEFT. origin of screen is at UPPERLEFT)

	LineSegment ray = App->camera->GenerateRaycast((float)vpX, (float)vpY);

	//check collisions
		// collisions with octree + aabb
	std::list<GameObject*> intersectedObjects;
	octree.GetRoot()->CollectLineIntersections(ray, intersectedObjects); //return map(distancia,gameobject)

	GameObject* pickedGO = nullptr;
	if (!intersectedObjects.empty())
	{
		// collisions with vertices
		float minDist = 3.40282e+038f; // max float
		float objDist = 3.40282e+038f;
		for (std::list<GameObject*>::const_iterator it = intersectedObjects.begin(); it != intersectedObjects.end(); ++it)
		{
			CMesh* cmesh = (*it)->GetComponent<CMesh>();
			unsigned int* indices = cmesh->GetIndices();
			float* vertices = cmesh->GetVertices();

			// transform ray to local mesh coordinates
			LineSegment localSpaceRay(ray);
			localSpaceRay.Transform((*it)->GetModelMatrix().Inverted());

			// generate each face of the mesh
			float3 face[3];
			float vtxDist;
			for (unsigned int i = 0; i < cmesh->GetNumIndices(); i += 3)
			{
				face[0].x = vertices[indices[i] * cmesh->GetVtxSize()];
				face[0].y = vertices[indices[i] * cmesh->GetVtxSize()+1];
				face[0].z = vertices[indices[i] * cmesh->GetVtxSize()+2];

				face[1].x = vertices[indices[i+1] * cmesh->GetVtxSize()];
				face[1].y = vertices[indices[i+1] * cmesh->GetVtxSize()+1];
				face[1].z = vertices[indices[i+1] * cmesh->GetVtxSize()+2];

				face[2].x = vertices[indices[i+2] * cmesh->GetVtxSize()];
				face[2].y = vertices[indices[i+2] * cmesh->GetVtxSize()+1];
				face[2].z = vertices[indices[i+2] * cmesh->GetVtxSize()+2];

				// compare the face with the ray
				Triangle f = Triangle(face[0], face[1], face[2]);
				if (localSpaceRay.Intersects(f, &vtxDist, nullptr))
				{
					// min distance
					if (vtxDist < objDist)
					{
						objDist = vtxDist;
					}
				}
			}
			// choose the GO selection by minDist
			
			if (objDist < minDist)
			{
				minDist = objDist;
				pickedGO = (*it);
			}
		}
	}
	if (pickedGO)
	{
		App->sceneMng->SetSelectedGO(pickedGO);
	}
}
