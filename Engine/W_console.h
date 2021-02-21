#pragma once
#include "Window.h"
#include <ctype.h>

class WConsole : public Window
{
private:
    char                  InputBuf[256];                    // Buffer that captures keyboard input from the user
    ImVector<char*>       Items;                            // Vector of each line that is being printed in the console
    ImVector<const char*> Commands;                         // Vector of commands available in the console
    ImGuiTextFilter       Filter;                           // Filter for the Items that will be displayed
    bool                  AutoScroll;                       // Indicates if the WConsole automatically scrolls down on each new Item to print, if the space is not enough to fit every line
    bool                  ScrollToBottom;                   // It changes state twice each frame (when AutoScroll is active), to control the distance the window must be scrolled

public:
    WConsole(std::string _name);                             // Constructor
    ~WConsole();                                            // Destructor

    void    Draw() override;                                // Operations performed when Rendering this window

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2);     // Adds a new Item
    void    ClearLog();                                     // Empties the Items vector

    void    ExecCommand(const char* _command_line);          // Processes the Input Buffer, identifies if any command has been written, and Executes this command
    static void  Strtrim(char* _s) { char* str_end = _s + strlen(_s); while (str_end > _s && str_end[-1] == ' ') str_end--; *str_end = 0; } // Gets the string written in the InputBuf
};