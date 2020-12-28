#include "ModuleSceneManager.h"
#include "ModuleProgram.h"

ModuleSceneManager::ModuleSceneManager()
{
	root->ChangeName("Scene 1");
}

ModuleSceneManager::~ModuleSceneManager()
{
	CleanUp();
}

void ModuleSceneManager::CreateEmptyGameObject()
{
	GameObject* newEmpty = new GameObject("Empty GameObj");
	newEmpty->SetParent(root);
}

bool ModuleSceneManager::Start()
{
	vanillaProgram = ModuleProgram::CreateProgramFromFile(".\\resources\\shaders\\vanilla.vs.glsl", ".\\resources\\shaders\\vanilla.fs.glsl");
	phongProgram = ModuleProgram::CreateProgramFromFile(".\\resources\\shaders\\phong.vs.glsl", ".\\resources\\shaders\\phong.fs.glsl");
	pbrProgram = ModuleProgram::CreateProgramFromFile(".\\resources\\shaders\\bdrfPhong.vs.glsl", ".\\resources\\shaders\\bdrfPhong.fs.glsl");
	defaultProgram = pbrProgram;
	return true;
}

bool ModuleSceneManager::CleanUp()
{
	glDeleteProgram(vanillaProgram);
	glDeleteProgram(phongProgram);
	glDeleteProgram(pbrProgram);

	RELEASE(root);
	return true;
}
