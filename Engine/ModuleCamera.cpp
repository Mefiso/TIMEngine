#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "Math/Quat.h"
#include "Leaks.h"


ModuleCamera::ModuleCamera(float3 position, float3 up, float yaw, float pitch, float near_plane, float far_plane) : Module(),
	Front(-float3::unitZ), MovementSpeed(SPEED), RotationSpeed(ROTATION_SPEED), MouseSensitivity(SENSITIVITY), aspectRatio(ASPECTRATIO),
	VFOV(VERTICALFOV), Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch), nearPlane(near_plane), farPlane(far_plane)
	
{
	HFOV = 2.f * Atan(Tan(VFOV * 0.5f) * aspectRatio);
}

// Destructor
ModuleCamera::~ModuleCamera()
{
}

// Called before Camera is available
bool ModuleCamera::Init()
{
	Front.Normalize();
	Right = Cross(Front, WorldUp).Normalized();
	Up = Cross(Right, Front).Normalized();

	// init frustum
	UpdateFrustum();

	return true;
}

// Called every draw update
update_status ModuleCamera::PreUpdate()
{	

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	LOG("Destroying Camera");

	return true;
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
	float celerity = MovementSpeed * deltaTime;
	float oldPitch; // Only needed if using RotateCamera
	switch (direction)
	{
	case FORWARD:
		Position += Front * celerity;
		break;
	case BACKWARD:
		Position -= Front * celerity;
		break;
	case LEFT:
		Position -= Right * celerity;
		break;
	case RIGHT:
		Position += Right * celerity;
		break;
	case UP:
		Position += WorldUp * celerity;
		break;
	case DOWN:
		Position -= WorldUp * celerity;
		break;
	case PITCH_UP:
		oldPitch = Pitch; // Only needed if using RotateCamera
		Pitch += RotationSpeed * celerity;
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		RotateCamera(0, RotationSpeed * celerity); //NewDirection();
		break;
	case PITCH_DOWN:
		oldPitch = Pitch; // Only needed if using RotateCamera
		Pitch -= RotationSpeed * celerity;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
		RotateCamera(0, -RotationSpeed * celerity); // NewDirection();
		break;
	case YAW_LEFT:
		Yaw -= RotationSpeed * celerity;
		RotateCamera(RotationSpeed * celerity, 0);//NewDirection();
		break;
	case YAW_RIGHT:
		Yaw += RotationSpeed * celerity;
		RotateCamera(-RotationSpeed * celerity, 0);//NewDirection();
		break;
	}
	UpdateFrustum();
}

void ModuleCamera::ProcessMouseMovement(float xoffset, float yoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	float oldPitch = Pitch; // Only needed if using RotateCamera

	Yaw += xoffset;
	Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	/*if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;
		*/
	// NewDirection();
	RotateCamera(-xoffset, yoffset);
	UpdateFrustum();
}

void ModuleCamera::ProcessMouseScroll(float yoffset)
{
	//Position += Front * yoffset; // simulates zoom but it's actually moving
	VFOV -= yoffset * ZOOM; // actual zoom
	UpdateFrustum();
}

void ModuleCamera::ProcessSpeed(float multiplier)
{
	MovementSpeed *= multiplier;
}

void ModuleCamera::ProcessOrbit(float xoffset, float yoffset, float3 orbit_centre)
{
	float3 lookDirection = Quat::RotateY(xoffset * MouseSensitivity) * (Position - orbit_centre);
	lookDirection = Quat::RotateAxisAngle(Right, yoffset * MouseSensitivity) * lookDirection;
	Position = lookDirection + orbit_centre;

	// Look
	Front = (-lookDirection).Normalized();
	Right = Cross(Front, WorldUp).Normalized();
	Up = Cross(Right, Front).Normalized();
	
	UpdateFrustum();
}

void ModuleCamera::onResize(float aspect_ratio)
{
	aspectRatio = aspect_ratio;
	UpdateFrustum();
}

void ModuleCamera::onCameraSettingsChanged()
{
	UpdateFrustum();
}

void ModuleCamera::UpdateFrustum()
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetVerticalFovAndAspectRatio(VFOV, aspectRatio);
	frustum.SetPos(Position);
	frustum.SetFront(Front);
	frustum.SetUp(Up);
	HFOV = frustum.HorizontalFov();
}

void ModuleCamera::RotateCamera(float yaw, float pitch)
{
	
	if (yaw != 0.f)
	{
		Quat yawRotation = Quat::RotateY(yaw);
		Front = yawRotation.Mul(Front).Normalized();
		Right = yawRotation.Mul(Right).Normalized();
		Up = yawRotation.Mul(Up).Normalized();
	}
	if (pitch != 0.f)
	{
		Quat pitchRotation = Quat(Right, pitch);
		float3 newUp = pitchRotation.Mul(Up).Normalized();
		if (newUp.y > 0) {
			Up = newUp;
			Front = pitchRotation.Mul(Front).Normalized();
			Right = pitchRotation.Mul(Right).Normalized();
		}
	}
}

void ModuleCamera::NewDirection()
{
	// Gramm-Schmidt process, not used if using RotateCamera

	// new front with simple trigonometry
	float3 front;
	front.x = cos(DegToRad(Yaw)) * cos(DegToRad(Pitch));
	front.y = sin(DegToRad(Pitch));
	front.z = sin(DegToRad(Yaw)) * cos(DegToRad(Pitch));
	Front = front.Normalized();
	Right = Cross(Front, WorldUp).Normalized(); // 6 scalar products and 3 subtractions + 3 scalar products, 2 sums and one sqrt
	Up = Cross(Right, Front).Normalized();
}
