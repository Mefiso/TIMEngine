#pragma once
#include "Window.h"
#include "GameObject.h"

#include <vector>

class WHierarchy : public Window
{
public:

private:
	GameObject* dragged = nullptr;
public:
	WHierarchy(std::string name);							// Constructor
	~WHierarchy();											// Destructor

	void Draw() override;									// Operations performed when Rendering this window
	void DrawTree(std::vector<GameObject*>& _gameObjList);	// Draws the gameObject Hierarchy Tree
	void DeselectAll(std::vector<GameObject*> goRoots);				// Resets the selection of GameObjects
};