#pragma once
#include "Module.h"
#include "Globals.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <vector>
#include <string>

class GameObject;
struct Texture;

class ModuleScene : public Module
{
public:
	std::vector<Texture*> loadedTextures;

private:
	std::vector<GameObject*> root;
	struct aiLogStream stream;
	std::string directory = "";

public:
	ModuleScene();
	~ModuleScene();

	//  ----- Module Functions ----- //
	bool Start() override;														// Load default scene
	bool CleanUp() override;													// Clean memory allocated by this Module
	void ReceiveEvent(const Event& event) override;								// Recieve events from App (that recieves events from other Modules)

	// ---------- Getters ---------- //
	const std::vector<GameObject*>& GetRoot() const { return root; }

private:
	void LoadScene(std::string const& path);									// Loads a model from a file indicated by _path, and creates the corresponding GameObjects and Components
	void ProcessNode(aiNode* node, const aiScene* scene, GameObject* object);	// Subfuction of LoadScene. Processes an Assimp Node from the loaded model and creates the corresponding GameObjects and Components from the node and its childs recursively
	void DropFile(const std::string& file);										// Called when a Drop File event is recieved. Calls Load Scene.
};