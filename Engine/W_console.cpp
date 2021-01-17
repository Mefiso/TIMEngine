#include "W_console.h"
#include "Application.h"
#include "ModuleWindow.h"
#include <stdlib.h>
#include <stdio.h>
#include "Leaks.h"

WConsole::WConsole(std::string _name) : Window(_name)
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));

	Commands.push_back("HELP");
	Commands.push_back("CLEAR");
	Commands.push_back("NAME");
	Commands.push_back("TRUENAME");
	AutoScroll = true;
	ScrollToBottom = true;
	AddLog("Welcome to Engine!");
}

WConsole::~WConsole()
{
	ClearLog();
}

void    WConsole::ClearLog()
{
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
}

void    WConsole::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
	// FIXME-OPT
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);

#ifdef _DEBUG
	bool alreadyIn = false;
	if (Items.size() > 4) {
		for (int i = Items.size() - 4; i < Items.size(); ++i) {
			if (strcmp(buf, Items[i]) == 0) {
				alreadyIn = true;
				break;
			}
		}
	}
	if (!alreadyIn)
		Items.push_back(_strdup(buf));
#else
	Items.push_back(_strdup(buf));
#endif // _DEBUG
}

void    WConsole::Draw()
{
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
	// So e.g. IsItemHovered() will return true when hovering the title bar.
	// Here we create a context menu only available from the title bar.
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Close Console"))
			active = false;
		ImGui::EndPopup();
	}

	//ImGui::Separator();

	// Options menu
	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &AutoScroll);
		ImGui::EndPopup();
	}

	// Options, Filter
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");
	ImGui::SameLine();
	Filter.Draw("Filter: (\"incl,-excl\") (\"error\") (\"-warning\")", 180);
	//ImGui::SameLine();
	//if (ImGui::GetWindowWidth() > 500)
	ImGui::Indent(ImGui::GetWindowWidth() - 100.f);
	if (ImGui::SmallButton("Clear")) { ClearLog(); }
	ImGui::SameLine();
	bool copy_to_clipboard = ImGui::SmallButton("Copy");
	ImGui::Unindent(ImGui::GetWindowWidth() - 100.f);
	ImGui::Separator();

	// Reserve enough left-over height for 1 separator + 1 input text
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear")) ClearLog();
		ImGui::EndPopup();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	if (copy_to_clipboard)
		ImGui::LogToClipboard();
	for (int i = 0; i < Items.Size; i++)
	{
		const char* item = Items[i];
		if (!Filter.PassFilter(item))
			continue;

		// Normally you would store more information in your item than just a string.
		// (e.g. make Items[] an array of structure, store color/type etc.)
		ImVec4 color;
		bool has_color = false;
		if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
		else if (strstr(item, "[info]")) { color = ImVec4(0.6f, 0.8f, 0.6f, 1.0f); has_color = true; }
		else if (strstr(item, "TIME")) { color = ImVec4(0.2f, 0.5f, 0.9f, 1.0f); has_color = true; }
		if (has_color)
			ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(item);
		if (has_color)
			ImGui::PopStyleColor();
	}
	if (copy_to_clipboard)
		ImGui::LogFinish();

	if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	ScrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, (ImGuiInputTextCallback)0, (void*)this))
	{
		char* s = InputBuf;
		Strtrim(s);
		if (s[0])
			ExecCommand(s);
		strcpy_s(s, 1, "");
		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

void    WConsole::ExecCommand(const char* _command_line)
{
	AddLog("# %s\n", _command_line);

	// Process command
	if (_stricmp(_command_line, "CLEAR") == 0)
	{
		ClearLog();
	}
	else if (_stricmp(_command_line, "HELP") == 0)
	{
		AddLog("Commands:");
		for (int i = 0; i < Commands.Size - 1; i++)
			AddLog("- %s", Commands[i]);
	}
	else if (_stricmp(_command_line, "NAME") == 0)
	{
		AddLog("This Game Engine is titled TIME or TIMEngine, which stands for The Incredible Mefiso's Engine.   There's actually another command: TRUENAME");
	}
	else if (_stricmp(_command_line, "TRUENAME") == 0)
	{
		AddLog("TIME: VGhlIEVuZ2luZSBuYW1lIGFjdHVhbGx5IHN0YW5kcyBmb3IgVHJlYWNoZXJvdXMgSW5mYW1vdXMgTWlzY3JlYW50IEVuZ2luZQkozL/iloDMv+KAv+KAicy/4paAzL8gzL8p");
	}
	else
	{
		AddLog("Unknown command: '%s'\n", _command_line);
	}

	// On command input, we scroll to bottom even if AutoScroll==false
	ScrollToBottom = true;
}