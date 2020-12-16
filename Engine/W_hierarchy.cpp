#include "W_hierarchy.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
//#include "W_properties.h"

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

	ImGui::SetNextItemOpen(true);
	if (ImGui::TreeNode(App->scene->GetRoot()->GetName()))
	{
		// Process Draging objects into Scene.root
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHYNODES"))
			{
				dragged->SetParent(App->scene->GetRoot());
			}
			ImGui::EndDragDropTarget();
		}

		// Draw Hierachy tree
		DrawTree(App->scene->GetRoot()->GetChildren());

		ImGui::TreePop();
	}
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

		// Process Selection of Items
		if (ImGui::IsItemClicked()) {
			if (!ImGui::GetIO().KeyCtrl) {
				DeselectAll(App->scene->GetRoot()->GetChildren());
				App->editor->InspectObject(_gameObjList[i]);   // send the selected GO to w_properties
			}
			_gameObjList[i]->isSelected = true;
		}

		// Process Dragging items and changing parents
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("HIERARCHYNODES", NULL, 0);
			dragged = _gameObjList[i];
			ImGui::Text("%s", _gameObjList[i]->GetName());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHYNODES"))
			{
				// TODO: Multiple dragging
				// TODO: Reordering
				dragged->SetParent(_gameObjList[i]);
			}
			ImGui::EndDragDropTarget();
		}

		// Process recusive tree
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