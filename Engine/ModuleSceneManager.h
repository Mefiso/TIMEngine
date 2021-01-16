#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "Octree.h"
#include "Config.h"

class ModuleSceneManager : public Module
{
public:
	Octree octree = Octree();										// Octree structure of the scene
	//Config config = Config();										// Config file for the scene

private:
	GameObject* root = new GameObject();							// Root GameObject of the scene. All loaded and new GameObjects will be hanging from 'root' as its children
	unsigned int skyboxVAO = 0u, skyboxVBO = 0u;					// IDs of the Skybox Cube buffer objects
	unsigned int skyboxTexture = 0u;								// ID of the cubemap texture generated for the skybox

	//  ----- Shading Programs ----- //
	unsigned int defaultProgram = 0u;								// ID of the default Shading program that will be used to render the scene. One of the following  Programs will be assigned as default
	unsigned int vanillaProgram = 0u;								// ID of the basic Shading program
	unsigned int phongProgram = 0u;									// ID of the Shading program using Phong
	unsigned int pbrProgram = 0u;									// ID of the Shading program using BDRF Phong and Fresnel
	unsigned int skyboxShader = 0u;									// ID of the Shading program of the Skybox Cubemap

public:
	ModuleSceneManager();											// Constructor
	~ModuleSceneManager();											// Destructor

	void CreateEmptyGameObject();									// Creates a new GameObject, and adds it as a child of 'root'
	void InitSkyboxVAO();											// Initialises the Skybox VBO and VAO
	void DrawSkybox();												// Sends the Skybox VAO to render

	//  ----- Module Functions ----- //
	bool Start() override;											// Start Module
	bool CleanUp() override;										// Clean memory allocated by this Module

	// ---------- Getters ---------- //
	GameObject* GetRoot() const { return root; }
	unsigned int GetProgram() const { return defaultProgram; }
	unsigned int GetSkyboxProgram() const { return skyboxShader; }
	unsigned int GetSkyboxTexture() const { return skyboxTexture; }

	// ---------- Setters ---------- //
	void SetSkyboxTexture(unsigned int _texID) { skyboxTexture = _texID; }
	void SetRoot(GameObject* _root) { RELEASE(root); root = _root; }

};