#pragma once
#include "Component.h"
#include "Globals.h"
#include "GameObject.h"
#include "CTransform.h"
#include "Geometry/Frustum.h"
#include "Math/float4x4.h"
#include "Math/float3x4.h"
#include "Math/Quat.h"

class CCamera : public Component
{
private:
	Frustum* frustum = new Frustum();		// frustrum object of this Camera

public:
	CCamera(GameObject* _owner);	// Constructor
	~CCamera();						// Destructor

	// ---------- Getters ---------- //
	Frustum* GetFrustum() { return frustum; }

	void UpdateFrustumFromTransform(CTransform* _transform);
	void UpdateTransformFromFrustum();
};
