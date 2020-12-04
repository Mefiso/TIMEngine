#pragma once
#include "Globals.h"
#include "ImGUI/imgui.h"
#include <string>

class Window
{
public:
	Window(std::string name);
	virtual ~Window();

	virtual void Draw() = 0;

	void Enable(bool active);		// Toggles the visibility of an ImGui Window

	bool isEnabled() const { return active; }
	const char* GetWindowName() const { return name.c_str(); }
protected:
	bool active = true;				// Indicates if the ImGui Window is visible
	std::string name;				// Name of the ImGui Window
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