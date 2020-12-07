#pragma once
#include "Component.h"
#include "ModuleTexture.h"

class CMaterial : public Component
{
public:
	std::vector<Texture*> textures;

public:
	CMaterial(GameObject* _owner, std::vector<Texture*>& _textures);
	~CMaterial();
};