#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL.h"
#include "CCamera.h"
#include <Geometry/Frustum.h>
#include <Geometry/LineSegment.h>

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
const float SPEED		= 2.5f;													// Base value of MovementSpeed
const float SENSITIVITY	= .005f;												// Base value of MouseSensitivity
const float VERTICALFOV	= DegToRad(60.f);										// Initial Vertical FOV of the camera
const float ASPECTRATIO	= SCREEN_WIDTH / (float)SCREEN_HEIGHT;					// Initial Aspect ratio of the camera Frustrum
//const float ZOOM = .1f;

class ModuleCamera : public Module
{
public:
	float MovementSpeed		= 0.f;												// Multiplier of the Camera movement speed when using the keyboard
	float MouseSensitivity	= 0.f;												// Multiplier of the Mouse sensitivity

	CCamera* defaultCamera	= new CCamera(nullptr);								// This is the default camera of the Engine. To simplify functionality, we use a Component Camera
	CCamera* activeCamera	= defaultCamera;									// The active camera is the one that will be used to render the scene. It can be changed to use a Gameobject with a Camera Component instead of the default camera
	Frustum* frustum		= defaultCamera->GetFrustum();						// Identifier of the Camera frustrum Object
	CCamera* cullingCamera	= activeCamera;										// The camera that performs the frustum culling, this should always be the active camera, but we allow to change it for debugging purposes

public:
	ModuleCamera();																// Constructor
	~ModuleCamera();															// Destructor

	// ------ Module Functions ----- //
	bool Start();																// Perform the initial frustum culling
	bool CleanUp() override;													// Clean memory allocated by this Module
	void ReceiveEvent(const Event& _event) override;							// Recieve events from App (that recieves events from other Modules)
	
	void			onFocus(float3 _center, float _distance);					// Moves the camera to Focus the selected object, and places it at a suitable distance
	void			onResize(float _aspect_ratio);								// Sets the camera FOV and aspectratio when resizing the application Window
	LineSegment		GenerateRaycast(float _x, float _y);						// Generates a LineSegment from a normalized x,y. Used for the mouse picking over the viewport.
	void			ProcessViewportEvents();									// Called from ModuleEditor, when an input is recieved inside the viewport. Perfoms the necessary operations for the corresponding input. This allows the control of the actions performed when the input is captured inside the viewport.
	
	// ---------- Getters ---------- //
	float4x4 ViewMatrix() const			{ return frustum->ViewMatrix(); }		// Returns the View matrix of the Camera frustrum
	float4x4 ProjectionMatrix() const	{ return frustum->ProjectionMatrix(); }	// Returns the Projection matrix of the Camera

	// ---------- Setters ---------- //
	void SetActiveCamera(CCamera* _camera)	{ activeCamera = _camera; frustum = _camera->GetFrustum(); }
	void ResetActiveCamera()				{ activeCamera = defaultCamera; frustum = defaultCamera->GetFrustum(); }
	void SetCullingCamera(CCamera* _camera)	{ cullingCamera = _camera; cullingCamera->PerformFrustumCulling(true); }
	void ResetCullingCamera()				{ cullingCamera = defaultCamera; cullingCamera->PerformFrustumCulling(true); }

	
private:
	// (Mouse) Event Handlers
	void ProcessMouseMovement(float _xoffset, float _yoffset);					// Applies the corresponding changes when recieving a Event::rotate_event from ModuleInput
	void ProcessOrbit(float _xoffset, float _yoffset, float3 _orbit_centre);	// Applies the corresponding changes when recieving a Event::orbit_event from ModuleInput
	void ProcessMouseScroll(float _xoffset, float _yoffset);					// Applies the corresponding changes when recieving a Event::wheel_event from ModuleInput

	// (Keyboard) Event Handlers
	void TranslateCamera(float _deltaTime) const;								// Moves the application camera if an Input event has occurred
	void RotateCameraKeys(float _deltaTime) const;								// Rotates the application camera if an Input event has occurred
	void ProcessSpeed(float _multiplier);										// Multiplies the current MovementSpeed by a factor
	void ProcessKeyboard(Camera_Movement _direction, float _deltaTime);			// Applies the corresponding changes when an input from keyboard is detected and the Viewport is hovered. (Editor.PreUpdate > Rendered.ProcessViewportEvents)

	void RotateCamera(float _yaw, float _pitch);								// Performs the actual rotation when called from ProcessMouseMOvement or ProcessKeyboard
};