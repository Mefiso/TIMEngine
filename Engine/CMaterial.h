#pragma once
#include "Component.h"
#include <vector>

struct Texture;
class CMaterial : public Component
{
public:
	std::vector<Texture*> textures;				// Vector of textures that are used to render the parent GameObject of this Component

public:
	CMaterial(GameObject* _owner);				// Constructor
	~CMaterial();								// Destructor
};