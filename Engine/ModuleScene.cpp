#include "ModuleScene.h"

#include "GL/glew.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "ModuleTexture.h"
#include "ModuleCamera.h"
#include "Component.h"
#include "GameObject.h"
#include "Leaks.h"

void AssimpLog(const char* msg, char* user) {
	if (msg)
		LOG("[info] Assimp Log: %s", msg);
}

ModuleScene::ModuleScene()
{
	stream.callback = AssimpLog;
	aiAttachLogStream(&stream);
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Start()
{
	LoadScene("./resources/models/baker_house/BakerHouse.fbx");
	return true;
}

bool ModuleScene::CleanUp()
{
	for (std::vector<Texture*>::iterator it = loadedTextures.begin(), end = loadedTextures.end(); it != end; ++it)
		glDeleteTextures(1, &((*it)->id));
	loadedTextures.clear();
	
	return true;
}

void ModuleScene::ReceiveEvent(const Event& event)
{
	switch (event.type)
	{
	case Event::file_dropped:
		DropFile(event.string.ptr);
		break;
	}
}


void ModuleScene::LoadScene(std::string const& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		LOG("[error] Error loading %s: %s", path, importer.GetErrorString());
	}
	else
	{
		directory = path.substr(0, path.find_last_of('/')).size() == path.size() ?
			path.substr(0, path.find_last_of('\\')) : path.substr(0, path.find_last_of('/'));
		
		GameObject* newModel = new GameObject();
		root.push_back(newModel);
		ProcessNode(scene->mRootNode, scene, newModel);
	}
}

void ModuleScene::ProcessNode(aiNode* node, const aiScene* scene, GameObject* object)
{
	object->AddComponent(TRANSFORM, nullptr);
	object->SetHasTransform(true);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		object->AddComponent(MESH, (void*) mesh);
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		object->AddComponent(MATERIAL, (void*) material, directory); 
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		GameObject* newChild = new GameObject();
		ProcessNode(node->mChildren[i], scene, newChild);
		newChild->SetParent(object);
	}
}

void ModuleScene::DropFile(const std::string& file)
{
	if (file.substr(file.find_last_of('.'), file.size()).compare(".fbx") == 0) {
		LoadScene(file);
		// What if new scene has no transform?
		//App->camera->onFocus((*root.end())->GetModelMatrix().Col3(3), 50);
	}
	else {
		// Replace all textures because we only have one per model
		//modelLoaded->ReloadTexture(file.c_str());
	}
}
