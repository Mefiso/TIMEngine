#pragma once
#include "Component.h"
#include "Math/float3.h"
#include "Math/float4x4.h"

class CTransform : public Component
{
private:
	float3 scale;
	float3 rotation;
	float3 position;

	float4x4 transformMatrix = float4x4::identity;							// Transform matrix generated from Rotation, Scale and Position

public:
	CTransform(GameObject* _owner);											// Constructor
	~CTransform();															// Destructor

	// ---------- Getters ---------- //
	const float3& GetPos() const { return position; }
	const float3& GetRotation() const { return rotation; }
	const float3& GetScale() const { return scale; }
	float4x4 GetTransformationMatrix() const { return transformMatrix; }

	// ---------- Setters ---------- //
	void SetPos(float3& _position) { position = _position; }
	void SetRotation(float3& _rotation) { rotation = _rotation; }
	void SetScale(float3& _scale) { scale = _scale; }

	void UpdateTransformMatrix();											// Updates transformMatrix when a change is detected in Rotation, Scale and Position
};