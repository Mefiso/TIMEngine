#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
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

// Called before quitting
bool ModuleCamera::CleanUp()
{
	LOG("Destroying Camera");

	return true;
}

void ModuleCamera::ReceiveEvent(const Event& event)
{
	switch (event.type)
	{
	case Event::rotate_event:
		ProcessMouseMovement((float)event.point2d.x, (float)event.point2d.y);
		break;
	case Event::orbit_event:
	{
		const GameObject* selected = App->editor->GetSelectedObject();
		if (selected)
			ProcessOrbit((float)event.point2d.x, (float)event.point2d.y, selected->GetModelMatrix().Col3(3));
		else
			ProcessOrbit((float)event.point2d.x, (float)event.point2d.y, float3::zero);
		break;
	}
	case Event::wheel_event:
		ProcessMouseScroll((float)event.point2d.x, (float)event.point2d.y);
		break;
	}
}

void ModuleCamera::ProcessViewportEvents() {
	TranslateCamera(deltatime);
	RotateCameraKeys(deltatime);
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		const GameObject* selected = App->editor->GetSelectedObject();
		if (selected)
			onFocus(selected->GetModelMatrix().Col3(3), selected->GetAABB().Size().Length() * 2.f);
		else
			onFocus(float3::zero, 20.0f);
	}
}

void ModuleCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float MoveCelerity = MovementSpeed * deltaTime;
	switch (direction)
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
}

void ModuleCamera::ProcessMouseMovement(float xoffset, float yoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;
	RotateCamera(-xoffset, yoffset);

	activeCamera->UpdateTransformFromFrustum();
}

void ModuleCamera::ProcessMouseScroll(float xoffset, float yoffset)
{
	frustum->SetPos(frustum->Pos() + frustum->Front() * yoffset); // simulates zoom but it's actually moving
	frustum->SetPos(frustum->Pos() + frustum->WorldRight() * xoffset);

	activeCamera->UpdateTransformFromFrustum();
}

void ModuleCamera::ProcessOrbit(float xoffset, float yoffset, float3 orbit_centre)
{
	float3 lookDirection = Quat::RotateAxisAngle(float3::unitY, xoffset * MouseSensitivity) * (frustum->Pos() - orbit_centre); // Another option would be to rotate around frustum.Up(), both appear to work for me
	lookDirection = Quat::RotateAxisAngle(frustum->WorldRight(), yoffset * MouseSensitivity) * lookDirection;
	frustum->SetPos(lookDirection + orbit_centre);

	// Look
	float3x3 look = float3x3::LookAt(frustum->Front(), (-lookDirection).Normalized(), frustum->Up(), float3::unitY);
	frustum->SetFront(look.MulDir(frustum->Front()).Normalized());
	frustum->SetUp(look.MulDir(frustum->Up()).Normalized());

	activeCamera->UpdateTransformFromFrustum();
}

void ModuleCamera::onResize(float aspect_ratio)
{
	frustum->SetVerticalFovAndAspectRatio(frustum->VerticalFov(), aspect_ratio);
}

void ModuleCamera::onFocus(float3 center, float distance)
{
	// The effect is that it moves in a perpendicular way with respect the camera front and then backwards/forward a distance
	frustum->SetPos(center);
	frustum->SetPos(center - frustum->Front() * distance);

	activeCamera->UpdateTransformFromFrustum();
}

void ModuleCamera::RotateCamera(float yaw, float pitch)
{
	if (yaw != 0.f)
	{
		Quat yawRotation = Quat::RotateY(yaw);
		frustum->SetFront(yawRotation.Mul(frustum->Front()).Normalized());
		frustum->SetUp(yawRotation.Mul(frustum->Up()).Normalized());
	}
	if (pitch != 0.f)
	{
		Quat pitchRotation = Quat(frustum->WorldRight(), pitch);
		float3 newUp = pitchRotation.Mul(frustum->Up()).Normalized();
		if (newUp.y > 0) {
			frustum->SetUp(newUp);
			frustum->SetFront(pitchRotation.Mul(frustum->Front()).Normalized());
		}
	}
}

void ModuleCamera::TranslateCamera(float deltaTime) const
{
	// Translate camera
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		App->camera->ProcessKeyboard(UP, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		App->camera->ProcessKeyboard(DOWN, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		App->camera->ProcessKeyboard(FORWARD, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		App->camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		App->camera->ProcessKeyboard(LEFT, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		App->camera->ProcessKeyboard(RIGHT, deltaTime);

	// Speed increase/decrease
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		App->camera->ProcessSpeed(2);
	}
	else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
		App->camera->ProcessSpeed(0.5f);
	}
}

void ModuleCamera::ProcessSpeed(float multiplier)
{
	MovementSpeed *= multiplier;
}

void ModuleCamera::RotateCameraKeys(float deltaTime) const
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->camera->ProcessKeyboard(PITCH_UP, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->camera->ProcessKeyboard(PITCH_DOWN, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->camera->ProcessKeyboard(YAW_LEFT, deltaTime);
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->camera->ProcessKeyboard(YAW_RIGHT, deltaTime);
}