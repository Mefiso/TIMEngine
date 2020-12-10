#pragma once

#include "Globals.h"
#include "Application.h"
#include "Component.h"
#include "Math/float4x4.h"
#include <vector>

class CTransform;
class CMaterial;

class GameObject
{
private:
	static int objectCount;																				// Counter of number of GameObjects. Used to set the uID of each new instance of GameObject.

public:

	char* name = "";

private:

	const int uID = objectCount;																		// Unique identifier of each GameObject instance
	GameObject* parent = nullptr;																		// Pointer to the parent of this GameObject
	std::vector<GameObject*> children;																	// Vector of pointers to GameObjects that are child of this GameObject
	std::vector<Component*> components;																	// Vector of Components attached to this GameObject
	bool isActive = true;																				// Indicates if this GameObject must be rendered or not
	bool hasTransform = true;																			// Indicates if this GameObject has a Transform COmponent // THIS SHOULD BE CONTROLLED IN CONSTRUCTOR and act accordignly
	CTransform* transform;																				// Direct pointer to the Transform Component


public:
		
	GameObject();																						// Constructor
	~GameObject();																						// Destructor

	void CleanUp();
	void Update();																						// Update this GameObject with the transformations applied to it. SHOULD THIS GO IN PREUPDATE STEP?
	void AddComponent(ComponentType _type, void* arg = nullptr, const std::string& path = "");			// Create and attach a new Component to this GameObject
	void RemoveComponent(int _cID);																		// Detach a component from this GameObject
	void RemoveChild(int childID);																		// Subfunction of SetParent(). Removes a GameObject (by ID) from this.children list (DOES NOT DELETE THE OBJECT)
	void AddChild(GameObject* _newChild);																// Subfunction of SetParent(). Places another GameObject as a child of this one
	

	// ---------- Getters ---------- //
	char* GetName() { return name; }
	CMaterial* GetMaterial() const;																			// Returns the Material Component of this GameObject, if there is one. If not, returns nullptr
	float4x4* GetModelMatrix() const;																	// Returns the global Model Matrix defined by the Transform Components of this GameObject and its parents
	// A getter for each kind of Component?

	// ---------- Setters ---------- //
	void ChangeName(char* _newName) { name = _newName; }
	void SetTransform();
	void SetParent(GameObject* _newParent);
	void SetProgram(unsigned int program);

};
