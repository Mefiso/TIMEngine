#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "Math/Quat.h"
#include "Math/float3x3.h"
#include "Leaks.h"


ModuleCamera::ModuleCamera(float3 position, float3 up, float near_plane, float far_plane) : Module(),
	MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(near_plane, far_plane);
	frustum.SetVerticalFovAndAspectRatio(VERTICALFOV, ASPECTRATIO);
	frustum.SetPos(position);
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(up);
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
		ProcessMouseMovement(event.point2d.x, event.point2d.y);
		break;
	case Event::orbit_event:
		ProcessOrbit(event.point2d.x, event.point2d.y, float3::zero);
		break;
	case Event::wheel_event:
		ProcessMouseScroll(event.point2d.x, event.point2d.y);
		break;
	case Event::window_resize:
	case Event::window_fullscreen:
		//onResize(event.point2d.x / (float) event.point2d.y);
		break;
	}
}

float4x4 ModuleCamera::ViewMatrix()
{
	float4x4 viewModelGL = frustum.ViewMatrix();
	return viewModelGL;// .Transposed();
}

float4x4 ModuleCamera::ProjectionMatrix()
{
	return frustum.ProjectionMatrix();// .Transposed();
}

void ModuleCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float MoveCelerity = MovementSpeed * deltaTime;
	switch (direction)
	{
	case FORWARD:
		frustum.SetPos(frustum.Pos() + frustum.Front() * MoveCelerity);
		break;
	case BACKWARD:
		frustum.SetPos(frustum.Pos() - frustum.Front() * MoveCelerity);
		break;
	case LEFT:
		frustum.SetPos(frustum.Pos() - frustum.WorldRight() * MoveCelerity);
		break;
	case RIGHT:
		frustum.SetPos(frustum.Pos() + frustum.WorldRight() * MoveCelerity);
		break;
	case UP:
		frustum.SetPos(frustum.Pos() + float3::unitY * MoveCelerity);
		break;
	case DOWN:
		frustum.SetPos(frustum.Pos() - float3::unitY * MoveCelerity);
		break;
	case PITCH_UP:
		RotateCamera(0, MoveCelerity/3);
		break;
	case PITCH_DOWN:
		RotateCamera(0, -MoveCelerity/3);
		break;
	case YAW_LEFT:
		RotateCamera(MoveCelerity/3, 0);
		break;
	case YAW_RIGHT:
		RotateCamera(-MoveCelerity/3, 0);
		break;
	}
}

void ModuleCamera::ProcessMouseMovement(float xoffset, float yoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	RotateCamera(-xoffset, yoffset);
}

void ModuleCamera::ProcessMouseScroll(float xoffset, float yoffset)
{
	frustum.SetPos(frustum.Pos() + frustum.Front() * yoffset); // simulates zoom but it's actually moving
	frustum.SetPos(frustum.Pos() + frustum.WorldRight() * xoffset);
	//frustum.SetVerticalFovAndAspectRatio( frustum.VerticalFov() - yoffset * ZOOM, frustum.AspectRatio()); // actual zoom
}

void ModuleCamera::ProcessSpeed(float multiplier)
{
	MovementSpeed *= multiplier;
}

void ModuleCamera::ProcessOrbit(float xoffset, float yoffset, float3 orbit_centre)
{
	float3 lookDirection = Quat::RotateAxisAngle(float3::unitY, xoffset * MouseSensitivity) * (frustum.Pos() - orbit_centre); // Another option would be to rotate around frustum.Up(), both appear to work for me
	lookDirection = Quat::RotateAxisAngle(frustum.WorldRight(), yoffset * MouseSensitivity) * lookDirection;
	frustum.SetPos(lookDirection + orbit_centre);

	// Look
	float3x3 look = float3x3::LookAt(frustum.Front(), (-lookDirection).Normalized(), frustum.Up(), float3::unitY);
	frustum.SetFront(look.MulDir(frustum.Front()).Normalized());
	frustum.SetUp(look.MulDir(frustum.Up()).Normalized());
}

void ModuleCamera::onResize(float aspect_ratio)
{
	frustum.SetVerticalFovAndAspectRatio(frustum.VerticalFov(), aspect_ratio);
}

void ModuleCamera::onFocus(float3 center, float distance)
{
	// The effect is that it moves in a perpendicular way with respect the camera front and then backwards/forward a distance
	frustum.SetPos(center);
	frustum.SetPos(center - frustum.Front() * distance);
}

void ModuleCamera::RotateCamera(float yaw, float pitch)
{
	
	if (yaw != 0.f)
	{
		Quat yawRotation = Quat::RotateY(yaw);
		frustum.SetFront(yawRotation.Mul(frustum.Front()).Normalized());
		frustum.SetUp(yawRotation.Mul(frustum.Up()).Normalized());
	}
	if (pitch != 0.f)
	{
		Quat pitchRotation = Quat(frustum.WorldRight(), pitch);
		float3 newUp = pitchRotation.Mul(frustum.Up()).Normalized();
		if (newUp.y > 0) {
			frustum.SetUp(newUp);
			frustum.SetFront(pitchRotation.Mul(frustum.Front()).Normalized());
		}
	}
}
