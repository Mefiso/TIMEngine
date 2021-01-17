#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleTimeManager.h"
#include "ModuleInput.h"
#include "ModuleSceneManager.h"
#include "Math/Quat.h"
#include "Math/float3x3.h"
#include "Leaks.h"

ModuleCamera::ModuleCamera() : Module(), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
{
	frustum->SetPos(float3(0, 1, 7));
}

// Destructor
ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Start()
{
	cullingCamera->PerformFrustumCulling(true);
	return true;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	LOG("Destroying Camera");
	RELEASE(defaultCamera);

	return true;
}

void ModuleCamera::ReceiveEvent(const Event& _event)
{
	switch (_event.type)
	{
	case Event::rotate_event:
		ProcessMouseMovement((float)_event.point2d.x, (float)_event.point2d.y);
		break;
	case Event::orbit_event:
	{
		const GameObject* selected = App->sceneMng->GetSelectedGO();
		if (selected)
			ProcessOrbit((float)_event.point2d.x, (float)_event.point2d.y, selected->GetModelMatrix().Col3(3));
		else
			ProcessOrbit((float)_event.point2d.x, (float)_event.point2d.y, float3::zero);
		break;
	}
	case Event::wheel_event:
		ProcessMouseScroll((float)_event.point2d.x, (float)_event.point2d.y);
		break;
	}
}

void ModuleCamera::onFocus(float3 _center, float _distance)
{
	// The effect is that it moves in a perpendicular way with respect the camera front and then backwards/forward a distance
	frustum->SetPos(_center);
	frustum->SetPos(_center - frustum->Front() * _distance);

	activeCamera->UpdateTransformFromFrustum();
	cullingCamera->PerformFrustumCulling(true);
}

void ModuleCamera::onResize(float _aspect_ratio)
{
	frustum->SetVerticalFovAndAspectRatio(frustum->VerticalFov(), _aspect_ratio);
}

LineSegment ModuleCamera::GenerateRaycast(float _x, float _y)
{
	LineSegment picking = frustum->UnProjectLineSegment(_x, _y);
	return picking;
}

void ModuleCamera::ProcessViewportEvents() {
	TranslateCamera(App->timeMng->GetRealTimeDeltaTime());
	RotateCameraKeys(App->timeMng->GetRealTimeDeltaTime());
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		const GameObject* selected = App->sceneMng->GetSelectedGO();
		if (selected)
		{
			float4 centerDistance = selected->ComputeCenterAndDistance();
			onFocus(centerDistance.xyz(), centerDistance.w);
		}
		else
			onFocus(float3::zero, 5000.0f);
	}
}

void ModuleCamera::ProcessMouseMovement(float _xoffset, float _yoffset)
{
	_xoffset *= MouseSensitivity;
	_yoffset *= MouseSensitivity;
	RotateCamera(-_xoffset, _yoffset);

	activeCamera->UpdateTransformFromFrustum();
	cullingCamera->PerformFrustumCulling(true);
}

void ModuleCamera::ProcessOrbit(float _xoffset, float _yoffset, float3 _orbit_centre)
{
	float3 lookDirection = Quat::RotateAxisAngle(float3::unitY, _xoffset * MouseSensitivity) * (frustum->Pos() - _orbit_centre); // Another option would be to rotate around frustum.Up(), both appear to work for me
	lookDirection = Quat::RotateAxisAngle(frustum->WorldRight(), _yoffset * MouseSensitivity) * lookDirection;
	frustum->SetPos(lookDirection + _orbit_centre);

	// Look
	float3x3 look = float3x3::LookAt(frustum->Front(), (-lookDirection).Normalized(), frustum->Up(), float3::unitY);
	frustum->SetFront(look.MulDir(frustum->Front()).Normalized());
	frustum->SetUp(look.MulDir(frustum->Up()).Normalized());

	activeCamera->UpdateTransformFromFrustum();
	cullingCamera->PerformFrustumCulling(true);
}

void ModuleCamera::ProcessMouseScroll(float _xoffset, float _yoffset)
{
	frustum->SetPos(frustum->Pos() + frustum->Front() * _yoffset * 200.f); // simulates zoom but it's actually moving
	frustum->SetPos(frustum->Pos() + frustum->WorldRight() * _xoffset * 200.f);

	activeCamera->UpdateTransformFromFrustum();
	cullingCamera->PerformFrustumCulling(true);
}

void ModuleCamera::TranslateCamera(float _deltaTime) const
{
	// Translate camera
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		App->camera->ProcessKeyboard(UP, _deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		App->camera->ProcessKeyboard(DOWN, _deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		App->camera->ProcessKeyboard(FORWARD, _deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		App->camera->ProcessKeyboard(BACKWARD, _deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		App->camera->ProcessKeyboard(LEFT, _deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		App->camera->ProcessKeyboard(RIGHT, _deltaTime);

	// Speed increase/decrease
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		App->camera->ProcessSpeed(2);
	}
	else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
		App->camera->ProcessSpeed(0.5f);
	}
}

void ModuleCamera::RotateCameraKeys(float _deltaTime) const
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->camera->ProcessKeyboard(PITCH_UP, _deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->camera->ProcessKeyboard(PITCH_DOWN, _deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->camera->ProcessKeyboard(YAW_LEFT, _deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->camera->ProcessKeyboard(YAW_RIGHT, _deltaTime);
}

void ModuleCamera::ProcessSpeed(float _multiplier)
{
	MovementSpeed *= _multiplier;
}

void ModuleCamera::ProcessKeyboard(Camera_Movement _direction, float _deltaTime)
{
	float MoveCelerity = MovementSpeed * _deltaTime;
	switch (_direction)
	{
	case FORWARD:
		frustum->SetPos(frustum->Pos() + frustum->Front() * MoveCelerity);
		break;
	case BACKWARD:
		frustum->SetPos(frustum->Pos() - frustum->Front() * MoveCelerity);
		break;
	case LEFT:
		frustum->SetPos(frustum->Pos() - frustum->WorldRight() * MoveCelerity);
		break;
	case RIGHT:
		frustum->SetPos(frustum->Pos() + frustum->WorldRight() * MoveCelerity);
		break;
	case UP:
		frustum->SetPos(frustum->Pos() + float3::unitY * MoveCelerity);
		break;
	case DOWN:
		frustum->SetPos(frustum->Pos() - float3::unitY * MoveCelerity);
		break;
	case PITCH_UP:
		RotateCamera(0, MoveCelerity / 3);
		break;
	case PITCH_DOWN:
		RotateCamera(0, -MoveCelerity / 3);
		break;
	case YAW_LEFT:
		RotateCamera(MoveCelerity / 3, 0);
		break;
	case YAW_RIGHT:
		RotateCamera(-MoveCelerity / 3, 0);
		break;
	}

	activeCamera->UpdateTransformFromFrustum();
	cullingCamera->PerformFrustumCulling(true);
}

void ModuleCamera::RotateCamera(float _yaw, float _pitch)
{
	if (_yaw != 0.f)
	{
		Quat yawRotation = Quat::RotateY(_yaw);
		frustum->SetFront(yawRotation.Mul(frustum->Front()).Normalized());
		frustum->SetUp(yawRotation.Mul(frustum->Up()).Normalized());
	}
	if (_pitch != 0.f)
	{
		Quat pitchRotation = Quat(frustum->WorldRight(), _pitch);
		float3 newUp = pitchRotation.Mul(frustum->Up()).Normalized();
		if (newUp.y > 0) {
			frustum->SetUp(newUp);
			frustum->SetFront(pitchRotation.Mul(frustum->Front()).Normalized());
		}
	}
}