#include "ImporterScene.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "Application.h"

#include "ModuleSceneManager.h"
#include "ModuleCamera.h"
#include "Component.h"
#include "GameObject.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <string>

// Assimp Callback
struct aiLogStream stream;		// Assimp logs are registered in this variable
void AssimpLog(const char* msg, char* user) {
	if (msg)
		LOG("[info] Assimp Log: %s", msg);
}

void ImporterScene::Import(std::string const& _path)
{
	stream.callback = AssimpLog;
	aiAttachLogStream(&stream);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		LOG("[error] Error loading %s: %s", _path, importer.GetErrorString());
	}
	else
	{
		std::string directory = _path.substr(0, _path.find_last_of('/')).size() == _path.size() ? _path.substr(0, _path.find_last_of('\\')) : _path.substr(0, _path.find_last_of('/'));
		std::string filename = _path.substr(0, _path.find_last_of('/')).size() == _path.size() ? _path.substr(_path.find_last_of('\\') + 1, _path.size()) : _path.substr(_path.find_last_of('/') + 1, _path.size());

		GameObject* newModel = new GameObject(filename.c_str());
		newModel->SetParent(App->sceneMng->GetRoot());
		ProcessNode(scene->mRootNode, scene, newModel, directory);

		// Set default shading program for this GameObject and all its children
		newModel->SetProgram(App->sceneMng->GetProgram());
	}
	importer.FreeScene();

	// TODO: What if new scene has no transform? (could it be possible?)
	float4 centerDistance = App->sceneMng->GetRoot()->GetChildren()[App->sceneMng->GetRoot()->GetChildren().size() - 1]->ComputeCenterAndDistance();
	App->camera->onFocus(centerDistance.xyz(), centerDistance.w);
	App->camera->cullingCamera->PerformFrustumCulling();
}

void ImporterScene::ProcessNode(aiNode* node, const aiScene* scene, GameObject* object, std::string _dir)
{
	object->AddComponent(TRANSFORM);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		// IMPORT MESHES
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::string meshPath = "./Library/Meshes/";
		meshPath = meshPath + node->mName.C_Str() + ".mesh";

		App->StartTimer();
		bool imported = ImporterMesh::Import(mesh, object, meshPath);
		LOG("IMPORT TIME: %d microseconds", App->StopTimer());
		// TODO: the if nesting shouldnt be included?
		if (imported)
		{
			// save the custom file format
			unsigned int fsize = ImporterMesh::Save(object->GetComponent<CMesh>(), meshPath.c_str());

			object->RemoveComponent(object->GetComponent<CMesh>()->GetUUID()); // empty the cmesh (THIS IS PROVISIONAL UNTIL THE FILESYSTEM IS CORRECTLY IMPLEMENTED) (we will rather import or load, but not both)
			if (fsize > 0)
			{
				if (object->AddComponent(MESH))
				{
					// load from custom file format
					App->StartTimer();
					ImporterMesh::Load(meshPath.c_str(), object, fsize);
					LOG("LOAD TIME: %d microseconds", App->StopTimer());
				}
			}
		}

		// IMPORT MATERIALS
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::string matPath = "./Library/Materials/";
		matPath = matPath + node->mName.C_Str() + ".material";

		App->StartTimer();
		imported = ImporterMaterial::Import(material, _dir, object);
		LOG("IMPORT TIME mat: %d microseconds", App->StopTimer());
		if (imported)
		{
			unsigned int fsize = ImporterMaterial::Save(object->GetComponent<CMaterial>(), matPath.c_str());

			object->RemoveComponent(object->GetComponent<CMaterial>()->GetUUID());
			if (fsize > 0)
			{
				if (object->AddComponent(MATERIAL))
				{
					App->StartTimer();
					ImporterMaterial::Load(matPath, object->GetComponent<CMaterial>(), fsize);
					LOG("LOAD TIME mat: %d microseconds", App->StopTimer());
				}
			}
		}
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		GameObject* newChild = new GameObject(node->mChildren[i]->mName.C_Str());
		ProcessNode(node->mChildren[i], scene, newChild, _dir);
		newChild->SetParent(object);
	}
}

bool ImporterScene::Load(const char* _path)
{
	// Read JSON
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _path, "rb")) != 0)
	{
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s", _path, strerror(err));
		return false;
	}
	else
	{
		App->sceneMng->SetSelectedGO(nullptr);
		App->sceneMng->CleanScene();

		char readBuffer[65536];
		rapidjson::FileReadStream is(f, readBuffer, sizeof(readBuffer));

		rapidjson::Document d;
		d.ParseStream(is);

		// Create gameobjects
		const rapidjson::Value& gameObjects = d["Game Objects"];
		std::map<int, GameObject*> orphans;
		for (int i = 0; i < gameObjects.Size(); ++i)
		{
			// Load GameObject
			GameObject* go = new GameObject(gameObjects[i]["Name"].GetString(), gameObjects[i]["UUID"].GetInt());
			if (gameObjects[i]["ParentUUID"].GetInt() != -1)
			{
				if (App->sceneMng->GetRoot()->GetUUID() == gameObjects[i]["ParentUUID"].GetInt())
					go->SetParent(App->sceneMng->GetRoot());
				else
				{
					GameObject* parent = App->sceneMng->GetRoot()->SearchChild(gameObjects[i]["ParentUUID"].GetInt());
					if (parent)
						go->SetParent(parent);
					else
						orphans[gameObjects[i]["ParentUUID"].GetInt()] = go;
				}
			}
			else
				App->sceneMng->SetRoot(go);

			// Load its components
			const rapidjson::Value& components = gameObjects[i]["Components"];
			for (int j = 0; j < components.Size(); ++j)
			{
				go->AddComponent(static_cast<ComponentType>(components[j]["Type"].GetInt()), components[j]["UUID"].GetInt());
				Component* c = go->GetComponent(components[j]["UUID"].GetInt());
				c->onLoad(components[j]);
			}
		}
		while (orphans.size() > 0)
		{
			for (std::map<int, GameObject*>::iterator it = orphans.begin(); it != orphans.end();)
			{
				GameObject* parent = App->sceneMng->GetRoot()->SearchChild((*it).first);
				if (parent)
				{
					(*it).second->SetParent(parent);
					orphans.erase(it);
				}
				else
					++it;
			}
		}

		fclose(f);

		// Set shaders program
		App->sceneMng->GetRoot()->SetProgram(App->sceneMng->GetProgram());

		// Frustum culling
		App->camera->cullingCamera->PerformFrustumCulling();
		return true;
	}

}

void ImporterScene::Save(const char* _filename)
{
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _filename, "wb")) != 0)
	{
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s", _filename, strerror(err));
		return;
	}
	else
	{
		char writeBuffer[65536];
		rapidjson::FileWriteStream os(f, writeBuffer, sizeof(writeBuffer));

		rapidjson::Document d;

		d.SetObject();
		rapidjson::Value gameObjects(rapidjson::kArrayType);

		App->sceneMng->GetRoot()->onSave(gameObjects, d);

		d.AddMember("Game Objects", gameObjects, d.GetAllocator());

		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		d.Accept(writer);

		fclose(f);
	}
}
