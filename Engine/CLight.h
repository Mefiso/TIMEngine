#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Math/float3.h"
#include "Math/float3x3.h"

class CLight : public Component
{
private:
	int lType = 0;						// 0 = Directional , 1 = Point, 2 = Spot Light
	float3 lColor = float3::one;		// Light color												- (ALL Lights)
	float lIntensity = 0.f;				// Strength of the light									- (ALL Lights)
	float Kc = 0.0001f;					// Constant Attenuation										- (Point | Spot Lights)
	float Kl = 0.0001f;					// Linera Attenuation										- (Point | Spot Lights)
	float Kq = 0.0001f;					// Quadratic Attenuation									- (Point | Spot Lights)
	float lInnerAng = 0.f;				// Angle where light intensity is constant					- (Spot Lights)
	float lOuterAng = 0.f;				// Intensity drops smoothly between inner and outer angle	- (Spot Lights)
	// Light position and direction are taken from the Transform Component of the GameObject
	float3 pos = owner->GetModelMatrix().TranslatePart();
	//float3 dir = owner->GetModelMatrix().RotatePart().Col3(2).Normalized();
	float3 dir = float3::one;

public:
	CLight(GameObject* _owner);			// Constructor
	~CLight();							// Destructor

	// ---------- Getters ---------- //
	int GetType() const { return lType; }
	float GetInnerAngle() const { return lInnerAng; }
	float GetOuterAngle() const { return lOuterAng; }
	int &GetTypeRef() { return lType; }
	float3& GetColorRef() { return lColor; }
	float& GetIntensityRef() { return lIntensity; }
	float& GetKcRef() { return Kc; }
	float& GetKlRef() { return Kl; }
	float& GetKqRef() { return Kq; }
	float& GetInnerAngRef() { return lInnerAng; }
	float& GetOuterAngRef() { return lOuterAng; }
	float3 GetPos() const { return pos; };
	float3 GetDir() const { return dir; }

	// ---------- Setters ---------- //
	void SetType(int _type) { lType = (_type < 0 || _type > 2) ? 0 : _type; }
	void SetOuterAngle(float _angle) { lOuterAng = _angle; }

};

