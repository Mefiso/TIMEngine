#pragma once
#include "Window.h"

enum GuizmoState {
	POS,
	ROTATION,
	SCALE
};
enum GuizmoMode {
	LOCAL,
	WORLD
};
enum GameState {
	PLAY,
	PAUSE,
	STOP
};

class WTools : public Window
{
private:
	GuizmoState guizmoState = POS;			// State of the ImGuizmo. To draw TRANSFORM, ROTATION or SCALE gizmos
	GuizmoMode guizmoMode = LOCAL;					// Mode of the gizmos to draw in LOCAL or WORLD coordinates
	GameState playPauseStop = STOP;					// Play, Pause and Stop buttons state

public:
	WTools(std::string name);			// Constructor
	~WTools();							// Destructor
	void Draw() override;				// Operations performed when Rendering this window

	// ---------- Getters ---------- //
	GuizmoState GetGuizmoState() const { return guizmoState; }
	GuizmoMode GetGuizmoMode() const { return guizmoMode; }
	GameState GetPlayPauseState() const { return playPauseStop; }
};