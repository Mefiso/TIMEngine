#include "ModuleFilesystem.h"

#include "ImporterScene.h"


#include "ModuleSceneManager.h" // Only for skybox loading. Should be removed when fixed hardcoding
#include "GL/glew.h"			// And this two should not be here
#include "ImporterMaterial.h"

ModuleFilesystem::ModuleFilesystem()
{
}

ModuleFilesystem::~ModuleFilesystem()
{
	CleanUp();
}

bool ModuleFilesystem::Start()
{
	ImporterScene::Import("./resources/models/baker_house/BakerHouse.fbx");
	std::vector<std::string> faces =
	{
		".\\resources\\skybox\\default\\right.jpg",
		".\\resources\\skybox\\default\\left.jpg",
		".\\resources\\skybox\\default\\top.jpg",
		".\\resources\\skybox\\default\\bottom.jpg",
		".\\resources\\skybox\\default\\front.jpg",
		".\\resources\\skybox\\default\\back.jpg"
	};
	App->sceneMng->SetSkyboxTexture(ImporterMaterial::LoadCubemap(faces));
	return true;
}

bool ModuleFilesystem::CleanUp()
{
	for (std::vector<Texture*>::iterator it = loadedTextures.begin(), end = loadedTextures.end(); it != end; ++it)
	{
		glDeleteTextures(1, &((*it)->id));
		RELEASE(*it);
	}
	loadedTextures.clear();

	return true;
}

void ModuleFilesystem::DropFile(const std::string& file)
{
	std::string ext = file.substr(file.find_last_of('.'), file.size());

	// FBX or OBJ files -> Load a scene
	if (ext.compare(".fbx") == 0 || ext.compare(".FBX") == 0 || ext.compare(".obj") == 0 || ext.compare(".OBJ") == 0)
		ImporterScene::Import(file);
}

