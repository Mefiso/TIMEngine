#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

class ModuleSceneManager : public Module
{

private:
	GameObject* root = new GameObject();							// Root GameObject of the scene. All loaded and new GameObjects will be hanging from 'root' as its children
	// Octree* ot = nullptr;										// Pointer to the

	//  ----- Shading Programs ----- //
	unsigned int defaultProgram = 0u;								// ID of the default Shading program that will be used to render the scene. One of the following  Programs will be assigned as default
	unsigned int vanillaProgram = 0u;								// ID of the basic Shading program
	unsigned int phongProgram = 0u;									// ID of the Shading program using Phong
	unsigned int pbrProgram = 0u;									// ID of the Shading program using BDRF Phong and Fresnel

public:
	ModuleSceneManager();											// Constructor
	~ModuleSceneManager();											// Destructor

	void CreateEmptyGameObject();									// Creates a new GameObject, and adds it as a child of 'root'

	//  ----- Module Functions ----- //
	bool Start() override;											// Start Module
	bool CleanUp() override;										// Clean memory allocated by this Module

	// ---------- Getters ---------- //
	GameObject* GetRoot() { return root; }
	unsigned int GetProgram() { return defaultProgram; }
};