#include "CMaterial.h"
#include "Application.h"
#include "Leaks.h"

CMaterial::CMaterial(GameObject* _owner) : Component(MATERIAL, _owner)
{
}

CMaterial::CMaterial(GameObject* _owner, const int _UUID) : Component(MATERIAL, _owner, _UUID)
{
}

CMaterial::~CMaterial()
{
	textures.clear();
}