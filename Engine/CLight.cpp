#include "CLight.h"

CLight::CLight(GameObject* _owner) : Component(LIGHT, _owner)
{
	SetPositionDirection(_owner->GetModelMatrix());
}

CLight::~CLight()
{
}

void CLight::SetPositionDirection(float4x4 _modelMatrix)
{
	dir = _modelMatrix.RotatePart().Col3(2).Normalized();
	pos = _modelMatrix.TranslatePart();
}
