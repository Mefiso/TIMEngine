#include "W_about.h"

WAbout::WAbout(std::string name, int ID) : Window(name, ID)
{
	libraries["glew"] = "2.1.0";
	libraries["SDL2"] = "2.0.12";
	libraries["ImGUI"] = "v1.79";
	libraries["MathGeoLib"] = "v1.5";
	libraries["DevIL"] = "1.8.0";
	libraries["assimp"] = "5.0.1";
	libraries["DebugDraw"] = "";
}

WAbout::~WAbout()
{
}

void WAbout::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(550, 300), ImGuiCond_Once);
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	ImVec4 color = { 0.2, 0.5, 1.0, 1.0 };
	ImGui::TextColored(color, "%s %s", engineName.c_str(), version.c_str());
	ImGui::Separator();
	ImGui::TextWrapped(description.c_str());
	ImGui::Separator();

	ImGui::Text("Authors: %s", authors.c_str());
	ImGui::TextUnformatted("Libraries used:");
	ImGui::SameLine();
	std::string libs = "";
	for (std::pair<std::string, std::string> l : libraries) {
		libs += l.first + " " + l.second.c_str() + ",";
	}
	ImGui::TextWrapped(libs.c_str());
	ImGui::Separator();

	ImGui::TextWrapped(license.c_str());
	ImGui::End();
}


