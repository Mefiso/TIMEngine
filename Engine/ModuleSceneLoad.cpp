#include "ModuleSceneLoad.h"
#include "ModuleSceneManager.h"

#include "ModuleTexture.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"

#include "Component.h"
#include "GameObject.h"
#include "Leaks.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>



void AssimpLog(const char* msg, char* user) {
	if (msg)
		LOG("[info] Assimp Log: %s", msg);
}

ModuleSceneLoad::ModuleSceneLoad()
{
	stream.callback = AssimpLog;
	aiAttachLogStream(&stream);
}

ModuleSceneLoad::~ModuleSceneLoad()
{
	CleanUp();
}

bool ModuleSceneLoad::Start()
{
	LoadScene("./resources/models/baker_house/BakerHouse.fbx");
	std::vector<std::string> faces =
	{
		".\\resources\\skybox\\default\\right.jpg",
		".\\resources\\skybox\\default\\left.jpg",
		".\\resources\\skybox\\default\\top.jpg",
		".\\resources\\skybox\\default\\bottom.jpg",
		".\\resources\\skybox\\default\\front.jpg",
		".\\resources\\skybox\\default\\back.jpg"
	};
	App->sceneMng->SetSkyboxTexture(ModuleTexture::LoadCubemap(faces));
	faces.empty();
	return true;
}

bool ModuleSceneLoad::CleanUp()
{
	for (std::vector<Texture*>::iterator it = loadedTextures.begin(), end = loadedTextures.end(); it != end; ++it)
	{
		glDeleteTextures(1, &((*it)->id));
		RELEASE(*it);
	}
	loadedTextures.clear();

	return true;
}

void ModuleSceneLoad::ReceiveEvent(const Event& event)
{
	switch (event.type)
	{
	case Event::file_dropped:
		DropFile(event.string.ptr);
		break;
	}
}

void ModuleSceneLoad::LoadScene(std::string const& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		LOG("[error] Error loading %s: %s", path, importer.GetErrorString());
	}
	else
	{
		directory = path.substr(0, path.find_last_of('/')).size() == path.size() ? path.substr(0, path.find_last_of('\\')) : path.substr(0, path.find_last_of('/'));
		std::string filename = path.substr(0, path.find_last_of('/')).size() == path.size() ? path.substr(path.find_last_of('\\')+1, path.size()) : path.substr(path.find_last_of('/')+1, path.size());

		GameObject* newModel = new GameObject(filename.c_str());
		newModel->SetParent(App->sceneMng->GetRoot());
		ProcessNode(scene->mRootNode, scene, newModel);

		// Set default shading program
		newModel->SetProgram(App->sceneMng->GetProgram());
		for (std::vector<GameObject*>::const_iterator it = newModel->GetChildren().begin(); it != newModel->GetChildren().end(); ++it) {
			(*it)->SetProgram(App->sceneMng->GetProgram());
		}
	}
	importer.FreeScene();
}

void ModuleSceneLoad::ProcessNode(aiNode* node, const aiScene* scene, GameObject* object)
{
	object->AddComponent(TRANSFORM, nullptr);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		object->AddComponent(MESH, (void*)mesh);
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		object->AddComponent(MATERIAL, (void*)material, directory);
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		GameObject* newChild = new GameObject(node->mChildren[i]->mName.C_Str());
		ProcessNode(node->mChildren[i], scene, newChild);
		newChild->SetParent(object);
	}
}

void ModuleSceneLoad::DropFile(const std::string& file)
{
	if ([file]() {
		const char* l[] = { ".fbx", ".FBX", ".obj" };
			for (int i = 0; i < sizeof(l) / sizeof(const char*); ++i)
			{
				if (file.substr(file.find_last_of('.'), file.size()).compare(l[i]) == 0)
					return true;
			}
		return false; }())
	{
		LoadScene(file);
		// TODO: What if new scene has no transform? (could it be possible?)
		float4 centerDistance = App->sceneMng->GetRoot()->GetChildren()[App->sceneMng->GetRoot()->GetChildren().size() - 1]->ComputeCenterAndDistance();
		App->camera->onFocus(centerDistance.xyz(), centerDistance.w);
	}
}