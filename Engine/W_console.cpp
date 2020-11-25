#include "W_console.h"
#include <stdlib.h>
#include <stdio.h>

WConsole::WConsole(std::string name, int ID) : Window(name, ID)
{
    ClearLog();
    memset(InputBuf, 0, sizeof(InputBuf));
    //HistoryPos = -1;

    
    Commands.push_back("HELP");
    Commands.push_back("CLEAR");
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
    Items.push_back(strdup(buf));
}

void    WConsole::Draw()
{
    ImGui::SetNextWindowSize(ImVec2(650, 370), ImGuiCond_Once);
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

    ImGui::TextWrapped(
        "Basic console log with filtering and copy functions.");

    if (ImGui::SmallButton("Clear")) { ClearLog(); }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

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
    Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
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
        else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
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
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void    WConsole::ExecCommand(const char* command_line)
{
    AddLog("# %s\n", command_line);

    // Process command
    if (stricmp(command_line, "CLEAR") == 0)
    {
        ClearLog();
    }
    else if (stricmp(command_line, "HELP") == 0)
    {
        AddLog("Commands:");
        for (int i = 0; i < Commands.Size; i++)
            AddLog("- %s", Commands[i]);
    }
    else
    {
        AddLog("Unknown command: '%s'\n", command_line);
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    ScrollToBottom = true;
}