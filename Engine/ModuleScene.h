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
	bool Start() override;											// Load default scene
	bool CleanUp() override;										// Clean memory allocated by this Module
	void ReceiveEvent(const Event& event) override;					// Recieve events from App (that recieves events from other Modules)

	// ---------- Getters ---------- //
	const std::vector<GameObject*>& GetRoot() const { return root; }

	void LoadScene(std::string const& path);
private:
	void ProcessNode(aiNode* node, const aiScene* scene, GameObject* object);
	void DropFile(const std::string& file);
};