#include "CMaterial.h"
#include "Application.h"
#include "Leaks.h"

CMaterial::CMaterial(GameObject* _owner) : Component(MATERIAL, _owner)
{
}

CMaterial::~CMaterial()
{
	textures.clear();
}