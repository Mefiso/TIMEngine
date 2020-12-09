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
	static int objectCount;								// Counter of number of GameObjects. Used to set the uID of each new instance of GameObject.

public:

	char* name = "";

private:

	const unsigned int uID = objectCount;				// Unique identifier of each GameObject instance
	GameObject* parent = nullptr;						// Pointer to the parent of this GameObject
	std::vector<GameObject*> children;					// Vector of pointers to GameObjects that are child of this GameObject
	std::vector<Component*> components;					// Vector of Components attached to this GameObject
	bool isActive = true;								// Indicates if this GameObject must be rendered or not
	bool hasTransform = true;							// Indicates if this GameObject has a Transform COmponent // THIS SHOULD BE CONTROLLED IN CONSTRUCTOR and act accordignly
	CTransform* transform;								// Direct pointer to the Transform Component


public:

	GameObject();										// Constructor
	~GameObject();										// Destructor

	void Update();										// Update this GameObject with the transformations applied to it. SHOULD THIS GO IN PREUPDATE STEP?
	void AddComponent(ComponentType _type, void* arg = nullptr, const std::string& path = "");			// Create and attach a new Component to this GameObject
	void RemoveComponent(unsigned int _cID);			// Detach a component from this GameObject
	void AddChild(GameObject* _newChild);				// Places another GameObject as a child of this one
	// Tindria mes sentit canviar el pare o el fill?
	void RemoveChild(/*GameObject* childtoRemove*/ int i);

	// ---------- Getters ---------- //
	char* GetName() { return name; }
	CMaterial* GetMaterial();
	float4x4* GetModelMatrix() const;
	// A getter for each kind of Component?

	// ---------- Setters ---------- //
	void ChangeName(char* _newName) { name = _newName; }
	void SetTransform();
	void SetParent(GameObject* _newParent) { parent = _newParent; }
	void SetProgram(unsigned int program);

};
