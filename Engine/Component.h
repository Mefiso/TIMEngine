#pragma once

class GameObject;

enum ComponentType {
	TRANSFORM,
	MESH,
	MATERIAL
};

class Component
{
private:

	static int componentCount;								// Global counter of number of Components. Used to set the uID of each new instance of Component.

public:

	const int ID = componentCount;							// Unique identifier of each Component instance

private:

	ComponentType type;										// Type of the Component (Defined by ComponentType enum)
	bool active = true;										// Indicates if the Component is active for rendering
protected:

	GameObject* owner = nullptr;							// Pointer to the GameObject this Component belongs


public:
	
	Component(ComponentType _type, GameObject* _owner);		// Constructor
	~Component();											// Destructor

	virtual void Update() {}								// Processes all modifications made to this Component each frame

	// ---------- Getters ---------- //
	ComponentType GetType() { return type; }
	bool IsActive() const { return active; }

	// ---------- Setters ---------- //
	void Enable() { active = true; }
	void Disable() { active = false; }
};
