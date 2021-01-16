#pragma once

#include "Globals.h"
#include "Application.h"
#include "Component.h"
#include "Math/float4x4.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "Algorithm/Random/LCG.h"
#include "debugdraw.h"
#include <vector>

class CTransform;
class CMaterial;

class GameObject
{
private:
	static LCG randomGen;																		// Random Generator used to set UUID.

public:
	static bool drawOBB;

private:

	std::string name = std::string("");
	const int UUID;																				// Unique identifier of each GameObject instance
	GameObject* parent = nullptr;																// Pointer to the parent of this GameObject
	std::vector<GameObject*> children;															// Vector of pointers to GameObjects that are child of this GameObject
	std::vector<Component*> components;															// Vector of Components attached to this GameObject
	bool isActive = true;																		// Indicates if this GameObject must be rendered or not
	CTransform* transform = nullptr;															// Direct pointer to the Transform Component
	AABB aabb;																					// AxisAlignedBoundingBox of the GameObject in local Coordinates of the object mesh
	OBB obb;																					// ObliquousBoundingBox of the GameObject in World Coordinates
	ddVec3 obbPoints[8];																		// World coordinates of the OBB vertices

public:

	GameObject();																				// Constructor
	GameObject(const std::string& _name);														// Constructor (name as param)
	GameObject(const std::string& _name, const int _UUID);
	~GameObject();																				// Destructor

	void CleanUp();																				// Clears all memory stored by this GameObject
	void Draw();																				// Update this GameObject with the transformations applied to it. SHOULD THIS GO IN PREUPDATE STEP?
	bool AddComponent(ComponentType _type, const int _UUID = -1);								// Create and attach a new Component to this GameObject
	void RemoveComponent(int _cID);																// Detach a component from this GameObject
	void AddChild(GameObject* _newChild);														// Subfunction of SetParent(). Places another GameObject as a child of this one
	void RemoveChild(int childID);																// Subfunction of SetParent(). Removes a GameObject (by ID) from this.children list (DOES NOT DELETE THE OBJECT)
	GameObject* SearchChild(int childID);

	// ---------- Getters ---------- //
	const std::string& GetName() const { return name; }
	int GetUUID() const { return UUID; }
	GameObject* GetParent() const { return parent; }
	std::vector<GameObject*>& GetChildren() { return children; }
	std::vector<Component*>& GetComponents() { return components; }
	CTransform* GetTransform() const { return transform; }										// Returns the Transform Component of this GameObject, if there is one. If not, returns nullptr
	float4x4 GetModelMatrix() const;															// Returns the global Model Matrix defined by the Transform Components of this GameObject and its parents
	const float3 GetAccumulatedScale() const;
	const AABB& GetAABB() const { return aabb; }
	const OBB& GetOBB() const { return obb; }

	Component* GetComponent(int _cID)
	{
		for (auto c : components) { if (c->GetUUID() == _cID) return c; }
		return nullptr;
	}

	template<typename T>
	T* GetComponent() const																		// Get a component of type T, or null if it does not exist on this GameObject
	{
		for (auto i : components) { T* c = dynamic_cast<T*>(i); if (c != nullptr) return c; }
		return nullptr;
	}

	template<typename T>
	std::vector<T*>& GetComponentsOfType()														// Get a vector of component of type T, empty if it does not exist on this GameObject
	{
		std::vector<T*> cs;
		for (auto i : components) { T* c = dynamic_cast<T*>(i); if (c != nullptr) cs.push_back(c); }
		return cs;
	}

	// ---------- Setters ---------- //
	void ChangeName(const std::string& _newName) { name = _newName; }
	void SetTransform(float3& _scale, float3& _rotation, float3& _translation);
	void SetTransform(float4x4& _newTransform);
	void SetParent(GameObject* _newParent);
	void SetProgram(unsigned int program);

	void UpdateBoundingBoxes();
	void UpdateOctreePosition();

	float4 ComputeCenterAndDistance() const;

	// Serialization
	void onSave(rapidjson::Value& config, rapidjson::Document& d);
};