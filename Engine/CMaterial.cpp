#include "CMaterial.h"

CMaterial::CMaterial(GameObject* _owner, std::vector<Texture*>& _textures) : Component(MATERIAL, _owner), textures(_textures)
{
}

CMaterial::~CMaterial()
{
}
