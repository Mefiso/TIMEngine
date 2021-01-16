#pragma once
#include "Component.h"
#include "Math/float3.h"

class CLight : public Component
{
private:
	int lType = 0;						// 0 = Directional , 1 = Point, 2 = Spot Light
	float3 lDir = float3::zero;			// Base direction of the light								- (Directional | Spot Lights)
	float3 lColor = float3::one;		// Light color												- (ALL Lights)
	float lIntensity = 0.f;				// Strength of the light									- (ALL Lights)
	float Kc = 0.0001f;					// Constant Attenuation										- (Point | Spot Lights)
	float Kl = 0.0001f;					// Linera Attenuation										- (Point | Spot Lights)
	float Kq = 0.0001f;					// Quadratic Attenuation									- (Point | Spot Lights)
	float lInnerAng = 0.f;				// Angle where light intensity is constant					- (Spot Lights)
	float lOuterAng = 0.f;				// Intensity drops smoothly between inner and outer angle	- (Spot Lights)

public:
	CLight(GameObject* _owner);			// Constructor
	~CLight();							// Destructor

	// ---------- Getters ---------- //
	int GetType() const { return lType; }
	int &GetTypeRef() { return lType; }
	float3& GetColorRef() { return lColor; }
	float& GetIntensityRef() { return lIntensity; }
	float3& GetDirectionRef() { return lDir; }
	float& GetKcRef() { return Kc; }
	float& GetKlRef() { return Kl; }
	float& GetKqRef() { return Kq; }
	float& GetInnerAngRef() { return lInnerAng; }
	float& GetOuterAngRef() { return lOuterAng; }

	// ---------- Setters ---------- //
	void SetType(int _type) { lType = (_type < 0 || _type > 2) ? 0 : _type; }

};

