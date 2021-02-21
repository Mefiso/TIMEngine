#include "W_tools.h"
#include "Application.h"
#include "ModuleTimeManager.h"
#include "ImporterScene.h"

WTools::WTools(std::string name) : Window(name)
{
}

WTools::~WTools()
{
}

void WTools::Draw()
{
	// TODO: hide tab bar
	// TODO. force window height
	if (!ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoDecoration))
	{
		ImGui::End();
		return;
	}
	ImGui::RadioButton("Transform", (int*)&guizmoState, 0); ImGui::SameLine();
	ImGui::RadioButton("Rotate", (int*)&guizmoState, 1); ImGui::SameLine();
	ImGui::RadioButton("Scale", (int*)&guizmoState, 2); ImGui::SameLine();
	ImGui::RadioButton("LocalAxis", (int*)&guizmoMode, 0); ImGui::SameLine();
	ImGui::RadioButton("WorldAxis", (int*)&guizmoMode, 1); ImGui::SameLine();
	ImGui::Indent(ImGui::GetWindowWidth() / 2.3f);
	if (ImGui::Button("Play"))
	{
		if (playPauseStop != PLAY)
		{
			if (playPauseStop == PAUSE)
				App->timeMng->ResumeGameClock();
			else
			{
				App->timeMng->StartGameClock();
				ImporterScene::Save("./Library/Scenes/temp.json");
			}
			playPauseStop = PLAY;
		}

	}
	ImGui::SameLine();

	if (ImGui::Button("Pause"))
	{
		App->timeMng->StopGameClock();
		playPauseStop = PAUSE;
	}
	ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		if (playPauseStop != STOP)
		{
			App->timeMng->StopGameClock();
			ImporterScene::Load("./Library/Scenes/temp.json");
			playPauseStop = STOP;
		}
	}
	ImGui::End();
}