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
	Frustum* frustum = new Frustum();							// frustrum object of this Camera

public:
	CCamera(GameObject* _owner);								// Constructor
	~CCamera();													// Destructor

	void Draw() override;										// Sends the information of this component to Render it

	// ---------- Getters ---------- //
	Frustum* GetFrustum() { return frustum; }

	void UpdateFrustumFromTransform(CTransform* _transform);	// Updates Frustum position and rotation. This function must be called when the Transform of the 'owner' GameObject changes
	void UpdateTransformFromFrustum();							// Updates the Transform of the 'owner' GameObject. This function must be called when the frustum is modified by the user inputs that control the camera
};
