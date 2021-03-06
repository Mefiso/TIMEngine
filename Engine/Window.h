#pragma once
#include "Globals.h"
#include "ImGUI/imgui.h"
#include <string>

class Window
{
public:
	Window(std::string name, int ID);
	virtual ~Window();

	virtual void Draw() = 0;

	void Enable(bool active);

	bool isEnabled() const { return active; }
	const char* GetWindowName() const { return name.c_str(); }
protected:
	bool active = true;
	int ID = -1;
	std::string name;
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