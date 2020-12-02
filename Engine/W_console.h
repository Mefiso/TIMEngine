#pragma once
#include "ImGUI/imgui.h"
#include "Window.h"
#include <ctype.h>

class WConsole : public Window
{
private:
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

public:
    WConsole(std::string name);
    ~WConsole();

    static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog();

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2);
    void    Draw() override;
    void    ExecCommand(const char* command_line);
};