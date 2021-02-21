#pragma once
#include "Window.h"
#include "GameObject.h"

#include <vector>

class WHierarchy : public Window
{
private:
	GameObject* dragged		= nullptr;						// Pointer to the GameObject that is being dragged (from BeginDragDropSource() ).
	GameObject* toDelete	= nullptr;						// Pointer to a GameObject to which the Delete option has been chosen, from the Context Menu (Right-Click)

public:
	WHierarchy(std::string _name);							// Constructor
	~WHierarchy();											// Destructor

	void Draw() override;									// Operations performed when Rendering this window
	void DrawTree(std::vector<GameObject*>& _gameObjList);	// Draws the gameObject Hierarchy Tree

	// ---------- Getters ---------- //
	GameObject* GetToDelete() const { return toDelete; }

	// ---------- Setters ---------- //
	void SetToDelete(GameObject* _gameObject) { toDelete = _gameObject; }
};