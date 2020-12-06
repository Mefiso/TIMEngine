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
	ComponentType type;
	bool active = true;
	GameObject* owner = nullptr;

public:
	
	Component(ComponentType _type, GameObject* _owner);
	~Component();

	void Enable() { active = true; }
	void Disable() { active = false; }
	bool IsActive() const { return active; }
	virtual void Update() {}
};