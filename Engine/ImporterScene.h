#pragma once
#include "Module.h"
#include "Globals.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <string>

class GameObject;

namespace ImporterScene
{
	void Import(std::string const& _path);															// Loads a model/scene from a file indicated by _path (.fbx, .obj, ...), and creates the corresponding GameObjects and Components
	void ProcessNode(aiNode* _node, const aiScene* _scene, GameObject* _object, std::string _dir);	// Subfuction of Import. Processes an Assimp Node from the loaded model and creates the corresponding GameObjects and Components from the node and its childs recursively
	bool Load(const char* _path);																	// Loads a Scene from a file in custom file format
	void Save(const char* _filename);																// Saves the current Scene in a file in custom file format
};