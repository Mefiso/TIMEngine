#include "W_hierarchy.h"

WHierarchy::WHierarchy(std::string name) : Window(name)
{
}

WHierarchy::~WHierarchy()
{
}

void WHierarchy::Draw()
{

	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

    if (ImGui::TreeNode("Selection State: Multiple Selection"))
    {
        HelpMarker("Hold CTRL and click to select multiple items.");
        static bool selection[5] = { false, false, false, false, false };
        for (int n = 0; n < 5; n++)
        {
            char buf[32];
            sprintf_s(buf, "Object %d", n);
            if (ImGui::Selectable(buf, selection[n]))
            {
                if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                    memset(selection, 0, sizeof(selection));
                selection[n] ^= 1;
            }
        }
        ImGui::TreePop();
    }

    // selectable treenode: https://github.com/ocornut/imgui/issues/190
    if (ImGui::TreeNode("Tree")) // For each child of GO ROOT
    {
        for (int x = 0; x < 3; x++)
        {
            bool open1 = ImGui::TreeNode((void*)(intptr_t)x, "Node%d", x);
            if (open1)
            {
                for (int y = 0; y < 3; y++) // For each child of GO there, this should be recursive
                {
                    bool open2 = ImGui::TreeNode((void*)(intptr_t)y, "Node%d.%d", x, y);
                    if (open2)
                    {
                        ImGui::Text("Even more contents");
                        if (ImGui::TreeNode("Tree in column"))
                        {
                            ImGui::Text("The quick brown fox jumps over the lazy dog");
                            ImGui::TreePop();
                        }
                    }
                    if (open2)
                        ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
	ImGui::End();

}
