#include "ModuleSceneManager.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"

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