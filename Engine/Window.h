#pragma once
#include "Globals.h"
#include "imgui.h"
#include <string>

class Window
{
protected:
	std::string name;					// Name of the ImGui Window
	bool active = true;					// Indicates if the ImGui Window is visible

public:
	Window(std::string name);			// Constructor
	virtual ~Window();					// Destructor

	virtual void Draw() = 0;			// Call Draw() for each Window
	void Enable(bool active);			// Toggles the visibility of an ImGui Window

	// ---------- Getters ---------- //
	bool isEnabled() const { return active; }
	const char* GetWindowName() const { return name.c_str(); }
};

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}