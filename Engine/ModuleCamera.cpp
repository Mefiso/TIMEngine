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
	//float oldPitch; // Only needed if using RotateCamera
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
		//oldPitch = Pitch; // Only needed if using RotateCamera
		Pitch += RotationSpeed * celerity;
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		NewDirection();//RotateCamera(Right, Pitch - oldPitch);
		break;
	case PITCH_DOWN:
		//oldPitch = Pitch; // Only needed if using RotateCamera
		Pitch -= RotationSpeed * celerity;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
		NewDirection();//RotateCamera(Right, Pitch - oldPitch);
		break;
	case YAW_LEFT:
		Yaw -= RotationSpeed * celerity;
		NewDirection();//RotateCamera(WorldUp, RotationSpeed * celerity);
		break;
	case YAW_RIGHT:
		Yaw += RotationSpeed * celerity;
		NewDirection();//RotateCamera(WorldUp, -RotationSpeed * celerity);
		break;
	}
	UpdateFrustum();
}

void ModuleCamera::ProcessMouseMovement(float xoffset, float yoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	//float oldPitch = Pitch; // Only needed if using RotateCamera

	Yaw += xoffset;
	Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	NewDirection();
	//RotateCamera(Right, Pitch-oldPitch);
	//RotateCamera(WorldUp, -xoffset);
	UpdateFrustum();
}

void ModuleCamera::ProcessMouseScroll(float yoffset)
{
	Position += Front * yoffset;
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
	App->editor->UpdateCameraSettings();
}

void ModuleCamera::RotateCamera(float3& axis, float angle)
{
	Quat rotationMatrix = Quat(axis, DegToRad(angle));
	Front = rotationMatrix * Front;
	Right = rotationMatrix * Right; // 9 scalar products and 6 sums
	Up = rotationMatrix * Up;
}

void ModuleCamera::NewDirection()
{
	// Gramm-Schmidt process, comment if using RotateCamera

	// new front with simple trigonometry
	float3 front;
	front.x = cos(DegToRad(Yaw)) * cos(DegToRad(Pitch));
	front.y = sin(DegToRad(Pitch));
	front.z = sin(DegToRad(Yaw)) * cos(DegToRad(Pitch));
	Front = front.Normalized();
	Right = Cross(Front, WorldUp).Normalized(); // 6 scalar products and 3 subtractions + 3 scalar products, 2 sums and one sqrt
	Up = Cross(Right, Front).Normalized();
}
