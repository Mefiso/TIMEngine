#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "Octree.h"
#include <list>

class ModuleSceneManager : public Module
{
public:
	Octree octree = Octree();										// Octree structure of the scene
	std::list<GameObject*> lightSources;							// List of all GO in the scene with a Light Component. This will be used in the shaders to only take the light of the closest objects.

private:
	const char*		sceneName		= "";
	GameObject*		root			= new GameObject("root");		// Root GameObject of the scene. All loaded and new GameObjects will be hanging from 'root' as its children
	GameObject*		selectedObject	= nullptr;						// Pointer to the selected Game Object. This will have the gizmos on him, w_properties will show its information, and will be highlighted in the w_hierarchy
	unsigned int	skyboxTexture	= 0u;							// ID of the cubemap texture generated for the skybox
	unsigned int	skyboxVAO = 0u, skyboxVBO = 0u;					// IDs of the Skybox Cube buffer objects


	//  ----- Shading Programs ----- //
	unsigned int defaultProgram	= 0u;								// ID of the default Shading program that will be used to render the scene. One of the following  Programs will be assigned as default
	unsigned int vanillaProgram	= 0u;								// ID of the basic Shading program
	unsigned int phongProgram	= 0u;								// ID of the Shading program using Phong
	unsigned int pbrProgram		= 0u;								// ID of the Shading program using BDRF Phong and Fresnel
	unsigned int skyboxShader	= 0u;								// ID of the Shading program of the Skybox Cubemap

public:
	ModuleSceneManager();											// Constructor
	~ModuleSceneManager();											// Destructor

	//  ----- Module Functions ----- //
	bool Start() override;											// Start Module
	bool CleanUp() override;										// Clean memory allocated by this Module

	void CreateEmptyGameObject();									// Creates a new GameObject, and adds it as a child of 'root'
	void InitSkyboxVAO();											// Initialises the Skybox VBO and VAO
	void DrawSkybox();												// Sends the Skybox VAO to render
	void MousePicker(int _x, int _y);								// Explores the octree to select an object on a mouse click in the viewport

	// ---------- Getters ---------- //
	const char*		GetName()					{ return sceneName; }
	GameObject*		GetRoot() const				{ return root; }
	GameObject*		GetSelectedGO() const		{ return selectedObject; };
	unsigned int	GetProgram() const			{ return defaultProgram; }
	unsigned int	GetSkyboxProgram() const	{ return skyboxShader; }
	unsigned int	GetSkyboxTexture() const	{ return skyboxTexture; }

	// ---------- Setters ---------- //
	void SetName(const char* _name)				{ sceneName = _name; }
	void SetSelectedGO(GameObject* _object)		{ selectedObject = _object; }
	void SetSkyboxTexture(unsigned int _texID)	{ skyboxTexture = _texID; }
	void SetRoot(GameObject* _root)				{ RELEASE(root); root = _root; }
};