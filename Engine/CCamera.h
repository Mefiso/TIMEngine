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
	Frustum* frustum = new Frustum();							// frustum object of this Camera
	float4 frustumPlanes[6];
	float3 frustumPoints[8];

public:
	CCamera(GameObject* _owner);								// Constructor
	~CCamera();													// Destructor

	void Draw() override;										// Sends the information of this component to Render it. If the owner GameObject is selected, it will draw the camera frustum.

	// ---------- Getters ---------- //
	Frustum* GetFrustum() { return frustum; }

	void UpdateFrustumFromTransform(CTransform* _transform);	// Updates Frustum position and rotation. This function must be called when the Transform of the 'owner' GameObject changes
	void UpdateTransformFromFrustum();							// Updates the Transform of the 'owner' GameObject. This function must be called when the frustum is modified by the user inputs that control the camera
	void PerformFrustumCulling(bool extractFrustum = false);	// Extracts the frustum planes and corner points if needed, and tells the renderer to perform the frustum culling
};
