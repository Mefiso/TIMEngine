#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <vector>
#include <string>

class GameObject;
struct Texture;

class ModuleScene : public Module
{
public:
	std::vector<Texture*> loadedTextures;										// A vector containing All the texures loaded

private:
	GameObject* root = new GameObject();										// Root GameObject of the scene. All loaded and new GameObjects will be hanging from 'root' as its children
	struct aiLogStream stream;													// Assimp logs are registered in this variable
	std::string directory = "";													// The path of the scene file we are loading, without filename

	//  ----- Shading Programs ----- //
	unsigned int defaultProgram = 0u;											// ID of the default Shading program that will be used to render the scene. One of the following  Programs will be assigned as default
	unsigned int vanillaProgram = 0u;											// ID of the basic Shading program
	unsigned int phongProgram = 0u;												// ID of the Shading program using Phong
	unsigned int pbrProgram = 0u;												// ID of the Shading program using BDRF Phong and Fresnel

public:
	ModuleScene();
	~ModuleScene();

	//  ----- Module Functions ----- //
	bool Start() override;														// Load default scene
	bool CleanUp() override;													// Clean memory allocated by this Module
	void ReceiveEvent(const Event& event) override;								// Recieve events from App (that recieves events from other Modules)

	// ---------- Getters ---------- //
	GameObject* GetRoot() { return root; }

	void CreateEmptyGameObject();												// Creates a new GameObject, and adds it as a child of 'root'

private:
	void LoadScene(std::string const& path);									// Loads a model from a file indicated by _path, and creates the corresponding GameObjects and Components
	void ProcessNode(aiNode* node, const aiScene* scene, GameObject* object);	// Subfuction of LoadScene. Processes an Assimp Node from the loaded model and creates the corresponding GameObjects and Components from the node and its childs recursively
	void DropFile(const std::string& file);										// Called when a Drop File event is recieved. Calls Load Scene.
};