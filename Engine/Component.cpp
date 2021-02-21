#include "Component.h"

LCG Component::randomGen = LCG();
Component::Component(ComponentType _type, GameObject* _owner) : type(_type), owner(_owner), UUID(randomGen.Int())
{
}

Component::Component(ComponentType _type, GameObject* _owner, const int _UUID) : type(_type), owner(_owner), UUID(_UUID)
{
}

Component::~Component()
{
}
