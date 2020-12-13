#include "W_hierarchy.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"

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

    std::vector<GameObject*> goRoots = App->scene->GetRoot();
    if (ImGui::TreeNode("SCENE N"))
    {
        DrawTree(goRoots);
        ImGui::TreePop();
    }

	ImGui::End();

}

void WHierarchy::DrawTree(std::vector<GameObject*> _gameObjList)
{
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    for (unsigned int i = 0u; i < _gameObjList.size(); ++i)
    {
       
        ImGuiTreeNodeFlags node_flags = base_flags;

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)_gameObjList[i]->GetUID(),
            node_flags | (_gameObjList[i]->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0) | (_gameObjList[i]->isSelected ? ImGuiTreeNodeFlags_Selected : 0),
            "%s", _gameObjList[i]->name);

        if (ImGui::IsItemClicked()) {
            if (!ImGui::GetIO().KeyCtrl)
                DeselectAll(App->scene->GetRoot());
            _gameObjList[i]->isSelected = !_gameObjList[i]->isSelected;
        }

        if (ImGui::BeginDragDropSource())
        {
            //TODO: DragnDrop functionality
            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
            ImGui::Text("This is a drag and drop source");
            ImGui::EndDragDropSource();
        }

        if (node_open)
        {
            std::vector<GameObject*> goChildren = _gameObjList[i]->GetChildren();
            DrawTree(goChildren);
            ImGui::TreePop();
        }
    }
}

void WHierarchy::DeselectAll(std::vector<GameObject*> goRoots)
{
    for (unsigned int i = 0u; i < goRoots.size(); ++i)
    {
        goRoots[i]->isSelected = false;
        std::vector<GameObject*> goChildren = goRoots[i]->GetChildren();
        DeselectAll(goChildren);
    }
}



/*
if (ImGui::TreeNode("Drag to reorder items (simple)"))
{
    // Simple reordering
    HelpMarker(
        "We don't use the drag and drop api at all here! "
        "Instead we query when the item is held but not hovered, and order items accordingly.");
    static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
    for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
    {
        const char* item = item_names[n];
        ImGui::Selectable(item);
        if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
        {
            int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
            if (n_next >= 0 && n_next < IM_ARRAYSIZE(item_names))
            {
                item_names[n] = item_names[n_next];
                item_names[n_next] = item;
                ImGui::ResetMouseDragDelta();
            }
        }
    }
    ImGui::TreePop();
}


*/
