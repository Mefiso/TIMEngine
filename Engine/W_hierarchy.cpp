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

    if (ImGui::TreeNode("SCENE N"))
    {
        DrawTree(App->scene->GetRoot());
        ImGui::TreePop();
    }

    const int COUNT = 5;
    static const char* items_data[COUNT] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
    static int items_list[COUNT] = { 0, 1, 2, 3, 4 };

    // Render + dragging
    for (int n = 0; n < COUNT; n++)
    {
        int item_no = items_list[n];
        ImGui::Selectable(items_data[item_no]);

        if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
        {
            float drag_dy = ImGui::GetMouseDragDelta(0).y;
            if (drag_dy < 0.0f && n > 0)
            {
                // Swap
                items_list[n] = items_list[n - 1];
                items_list[n - 1] = item_no;
                ImGui::ResetMouseDragDelta();
            }
            else if (drag_dy > 0.0f && n < COUNT - 1)
            {
                items_list[n] = items_list[n + 1];
                items_list[n + 1] = item_no;
                ImGui::ResetMouseDragDelta();
            }
        }
    }

   /* int[] butnum;
    for (int i = 1; i< 16; ++i)
        butnum[i] = i

    local anchor = {}
    function GL.imgui()

    for (int i = 1; i < 16; ++i)

        ig.Button("but"..butnum[i].."###"..i, ig.ImVec2(50, 50))

        if ig.BeginDragDropSource() then
            anchor.data = ffi.new("int[1]", i)
            ig.SetDragDropPayload("ITEMN", anchor.data, ffi.sizeof"int")--, C.ImGuiCond_Once);
            ig.Button("drag"..butnum[i], ig.ImVec2(50, 50));
            ig.EndDragDropSource();
            end
                if ig.BeginDragDropTarget() then
                    local payload = ig.AcceptDragDropPayload("ITEMN")
                    if (payload~= nil) then
                        assert(payload.DataSize == ffi.sizeof"int");
            local num = ffi.cast("int*", payload.Data)[0]
                local tmp = butnum[num]
                table.remove(butnum, num)
                table.insert(butnum, i, tmp)
                end
                ig.EndDragDropTarget();
            end
                if (((i - 1) % 4) < 3) then ig.SameLine() end
                    end
                    end*/

	ImGui::End();

}

void WHierarchy::DrawTree(std::vector<GameObject*>& _gameObjList)
{
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    for (unsigned int i = 0u; i < _gameObjList.size(); ++i)
    {
        ImGuiTreeNodeFlags node_flags = base_flags;

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)_gameObjList[i]->GetUID(),
            node_flags | (_gameObjList[i]->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0) | (_gameObjList[i]->isSelected ? ImGuiTreeNodeFlags_Selected : 0),
            "%s", _gameObjList[i]->GetName());

        if (ImGui::IsItemClicked()) {
            if (!ImGui::GetIO().KeyCtrl)
                DeselectAll(App->scene->GetRoot());
                // TODO: send the selected to w_properties
            _gameObjList[i]->isSelected = !_gameObjList[i]->isSelected;
        }

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("_HIERARCHYNODES", _gameObjList[i], sizeof(GameObject*));
            ImGui::Text("%s", _gameObjList[i]->GetName());
            ImGui::EndDragDropSource();
            LOG("%x", _gameObjList[i]);
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_HIERARCHYNODES"))
            {
                IM_ASSERT(payload->DataSize == sizeof(GameObject*));
                GameObject* selectedGO = (GameObject*)payload->Data;
                LOG("%s", selectedGO->GetName());
                
                selectedGO->SetParent(_gameObjList[i]);
            }
            ImGui::EndDragDropTarget();
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


// Reordering
      /*if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
      {
          float drag_dy = ImGui::GetMouseDragDelta(0).y;
          if (drag_dy < 0.0f && i > 0)
          {
              // Swapping Up
              GameObject* aux = _gameObjList[i];
              _gameObjList[i] = _gameObjList[i - 1];
              _gameObjList[i - 1] = aux;
              ImGui::ResetMouseDragDelta();
              LOG("Swapping up");
          }
          else if (drag_dy > 0.0f && i < _gameObjList.size() - 1)
          {
              // Swapping Down
              items_list[n] = items_list[n + 1];
              items_list[n + 1] = item_no;
              ImGui::ResetMouseDragDelta();
              LOG("Swapping down");
          }
      }*/


/*FLAGS
ImGuiDragDropFlags_None                         = 0,
    // BeginDragDropSource() flags
    ImGuiDragDropFlags_SourceNoPreviewTooltip       = 1 << 0,   // By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disable this behavior.
    ImGuiDragDropFlags_SourceNoDisableHover         = 1 << 1,   // By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disable this behavior so you can still call IsItemHovered() on the source item.
    ImGuiDragDropFlags_SourceNoHoldToOpenOthers     = 1 << 2,   // Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
    ImGuiDragDropFlags_SourceAllowNullID            = 1 << 3,   // Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear imgui ecosystem and so we made it explicit.
    ImGuiDragDropFlags_SourceExtern                 = 1 << 4,   // External source (from outside of dear imgui), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
    ImGuiDragDropFlags_SourceAutoExpirePayload      = 1 << 5,   // Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
    // AcceptDragDropPayload() flags
    ImGuiDragDropFlags_AcceptBeforeDelivery         = 1 << 10,  // AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
    ImGuiDragDropFlags_AcceptNoDrawDefaultRect      = 1 << 11,  // Do not draw the default highlight rectangle when hovering over target.
    ImGuiDragDropFlags_AcceptNoPreviewTooltip       = 1 << 12,  // Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.
    ImGuiDragDropFlags_AcceptPeekOnly               = ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect  // For peeking ahead and inspecting the payload before delivery.

*/