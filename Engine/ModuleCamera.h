#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL.h"
#include "Geometry/Frustum.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	PITCH_UP,
	PITCH_DOWN,
	YAW_LEFT,
	YAW_RIGHT
};

// Default values
const float SPEED = 2.5f;													// Base value of MovementSpeed
const float SENSITIVITY = .005f;											// Base value of MouseSensitivity
const float VERTICALFOV = DegToRad(60.f);									// Initial Vertical FOV of the camera
const float ASPECTRATIO = SCREEN_WIDTH / (float)SCREEN_HEIGHT;				// Initial Aspect ratio of the camera Frustrum
//const float ZOOM = .1f;

class ModuleCamera : public Module
{
public:
	float MovementSpeed = 0.f;												// Multiplier of the Camera movement speed when using the keyboard
	float MouseSensitivity = 0.f;											// Multiplier of the Mouse sensitivity
	Frustum frustum;														// Identifier of the Camera frustrum Object

private:
	float deltatime;														// Time between each frame, in milliseconds

public:
	ModuleCamera(float3 position = float3(0, 1, 7), float3 up = float3(0, 1, 0), float near_plane = 0.1f, float far_plane = 200.0f);	// Constructor
	~ModuleCamera();														// Destructor

	// ------ Module Functions ----- //
	bool CleanUp() override;												// Clean memory allocated by this Module
	void ReceiveEvent(const Event& event) override;							// Recieve events from App (that recieves events from other Modules)
	// callback funcs
	void ProcessViewportEvents();											// Called from ModuleEditor, when an input is recieved inside the viewport. Perfoms the necessary operations for the corresponding input. This allows the control of the actions performed when the input is captured inside the viewport.

	// ---------- Getters ---------- //
	float4x4 ViewMatrix() const { return frustum.ViewMatrix(); }			// Returns the View matrix of the Camera frustrum
	float4x4 ProjectionMatrix() const { return frustum.ProjectionMatrix(); }// Returns the Projection matrix of the Camera

	// ---------- Setters ---------- //
	void SetDeltaTime(float _deltaTime) { deltatime = _deltaTime; }

	// Process movement
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);		// Applies the corresponding changes when an input from keyboard is detected and the Viewport is hovered. (Editor.PreUpdate > Rendered.ProcessViewportEvents)
	void ProcessSpeed(float multiplier);									// Multiplies the current MovementSpeed by a factor
	void onResize(float aspect_ratio);										// Sets the camera FOV and aspectratio when resizing the application Window
	void onFocus(float3 center, float distance);							// Moves the camera to Focus the selected object, and places it at a suitable distance

private:
	// Process Camera Rotations
	void RotateCamera(float yaw, float pitch);								// Auxiliar function of ProcessKeyboard() when rotation is needed
	void RotateCameraKeys(float deltaTime) const;							// Rotates the application camera if an Input event has occurred
	void ProcessOrbit(float xoffset, float yoffset, float3 orbit_centre);	// Applies the corresponding changes when recieving a Event::orbit_event from ModuleInput

	// Process Camera Movements
	void TranslateCamera(float deltaTime) const;							// Moves the application camera if an Input event has occurred
	void ProcessMouseMovement(float xoffset, float yoffset);				// Applies the corresponding changes when recieving a Event::rotate_event from ModuleInput
	void ProcessMouseScroll(float xoffset, float yoffset);					// Applies the corresponding changes when recieving a Event::wheel_event from ModuleInput
};