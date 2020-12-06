#pragma once
#include "Component.h"
#include "Math/float3.h"
#include "Math/float4x4.h"

class CTransform : public Component
{
public:
	float3 scale;
	float3 rotation;
	float3 position;

public:
	CTransform(GameObject* _owner);
	~CTransform();

	float4x4 GetTransformationMatrix() const;
	void SetPos(float3& _position) { position = _position; }
	void SetRotation(float3& _rotation) { rotation = _rotation; }
	void SetScale(float3& _scale) { scale = _scale; }
};