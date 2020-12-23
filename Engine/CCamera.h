#pragma once
#include "Component.h"
#include "Globals.h"
#include "Geometry/Frustum.h"

class CCamera : public Component
{
private:
	Frustum* frustum = nullptr;		// frustrum object of this Camera

public:
	CCamera(GameObject* _owner);	// Constructor
	~CCamera();						// Destructor
};

