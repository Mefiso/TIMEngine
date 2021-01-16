#pragma once
#include "Window.h"

class WTools : public Window
{
private:
	int guizmoState = 0;				// State of the ImGuizmo. To draw TRANSFORM, ROTATION or SCALE gizmos
	int guizmoMode = 0;					// Mode of the gizmos to draw in LOCAL or WORLD coordinates
	int playPauseStop = 2;				// Play, Pause and Stop buttons state

public:
	WTools(std::string name);			// Constructor
	~WTools();							// Destructor
	void Draw() override;				// Operations performed when Rendering this window

	// ---------- Getters ---------- //
	int GetGuizmoState() const { return guizmoState; }
	int GetGuizmoMode() const { return guizmoMode; }
	int GetPlayPauseState() const { return playPauseStop; }
};