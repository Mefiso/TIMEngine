#pragma once
#include "Module.h"
#include "Globals.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <string>

class GameObject;

namespace ImporterScene
{
	void Load(std::string const& _path);															// Loads a model from a file indicated by _path, and creates the corresponding GameObjects and Components
	void ProcessNode(aiNode* _node, const aiScene* _scene, GameObject* _object, std::string _dir);	// Subfuction of LoadScene. Processes an Assimp Node from the loaded model and creates the corresponding GameObjects and Components from the node and its childs recursively
};

