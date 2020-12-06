#include "Component.h"

Component::Component(ComponentType _type, GameObject* _owner) : type(_type), owner(_owner)
{
}

Component::~Component()
{
}
