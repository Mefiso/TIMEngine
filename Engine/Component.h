#pragma once

#include "Algorithm/Random/LCG.h"
#include "rapidjson/document.h"

class GameObject;

enum ComponentType {
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	LIGHT,
	INVALID
};

class Component
{
private:
	static LCG randomGen;									// RNG for setting UUIDs.

public:
	const int UUID;											// Unique identifier of each Component instance

private:
	ComponentType	type	= INVALID;						// Type of the Component (Defined by ComponentType enum)
	bool			active	= true;							// Indicates if the Component is active for rendering
protected:
	GameObject*		owner	= nullptr;						// Pointer to the GameObject this Component belongs

public:
	Component(ComponentType _type, GameObject* _owner);		// Constructor
	Component(ComponentType _type, GameObject* _owner, const int _UUID);
	virtual ~Component();									// Destructor
	virtual void Draw() {}									// Sends the information of this component to Render it

	// ---------- Getters ---------- //
	ComponentType GetType() const	{ return type; }
	GameObject* GetOwner() const	{ return owner; }
	const int GetUUID() const		{ return UUID; }
	bool IsActive() const			{ return active; }

	// ---------- Setters ---------- //
	void Enable()	{ active = true; }
	void Disable()	{ active = false; }

	// ------ Serialization -------- //
	virtual void onSave(rapidjson::Value& config, rapidjson::Document& d) const {} 	// To serialize this component when saving the scene
	virtual void onLoad(const rapidjson::Value& config) {}							// To load this component when loading the scene
};
