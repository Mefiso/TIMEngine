#pragma once
#include "Component.h"
#include <vector>

struct Texture;
class CMaterial : public Component
{
public:
	std::vector<Texture*> textures;				// Vector of textures that are used to render the parent GameObject of this Component

	float3 ambient = float3(0.05);
	float3 diffuse = float3(0.5);
	float shininess = 64.0f;
	float3 specular = float3(0.08f);
	int shininessAlpha = 0;

public:
	CMaterial(GameObject* _owner);				// Constructor
	~CMaterial();								// Destructor
};