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

	static int objectCount;																				// GLobal counter of number of GameObjects. Used to set the uID of each new instance of GameObject.

public:

	char* name = "New GameObject";
	bool isSelected = false;

private:

	const int uID = objectCount;																		// Unique identifier of each GameObject instance
	GameObject* parent = nullptr;																		// Pointer to the parent of this GameObject
	std::vector<GameObject*> children;																	// Vector of pointers to GameObjects that are child of this GameObject
	std::vector<Component*> components;																	// Vector of Components attached to this GameObject
	bool isActive = true;																				// Indicates if this GameObject must be rendered or not
	CTransform* transform = nullptr;																	// Direct pointer to the Transform Component
	bool hasTransform = false;																			// Indicates if this GameObject has a Transform COmponent // THIS SHOULD BE CONTROLLED IN CONSTRUCTOR and act accordignly


public:

	GameObject();																						// Constructor
	~GameObject();																						// Destructor

	void CleanUp();
	void Update();																						// Update this GameObject with the transformations applied to it. SHOULD THIS GO IN PREUPDATE STEP?
	void AddComponent(ComponentType _type, void* arg = nullptr, const std::string& path = "");			// Create and attach a new Component to this GameObject
	void RemoveComponent(int _cID);																		// Detach a component from this GameObject
	void AddChild(GameObject* _newChild);																// Subfunction of SetParent(). Places another GameObject as a child of this one
	void RemoveChild(int childID);																		// Subfunction of SetParent(). Removes a GameObject (by ID) from this.children list (DOES NOT DELETE THE OBJECT)

	// ---------- Getters ---------- //
	char* GetName() const { return name; }
	const int GetUID() const { return uID; }
	std::vector<GameObject*> GetChildren() const { return children; }
	const bool HasTransform() const { return hasTransform; }
	float4x4 GetModelMatrix() const;																	// Returns the global Model Matrix defined by the Transform Components of this GameObject and its parents
	CMaterial* GetMaterial() const;																		// Returns the Material Component of this GameObject, if there is one. If not, returns nullptr
	// A getter for each kind of Component?

	// ---------- Setters ---------- //
	void ChangeName(char* _newName) { name = _newName; }
	void SetTransform();
	void SetParent(GameObject* _newParent);
	void SetProgram(unsigned int program);
	void SetHasTransform(bool _hasTransform) { hasTransform = _hasTransform; }
};
