#include "Component.h"

int Component::componentCount = 0;

Component::Component(ComponentType _type, GameObject* _owner) : type(_type), owner(_owner)
{
	componentCount++;
}

Component::~Component()
{
}
