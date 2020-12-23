#include "W_properties.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "CTransform.h"
#include "CMaterial.h"
#include "CMesh.h"
#include "GL/glew.h"
#include "Leaks.h"

WProperties::WProperties(std::string _name) : Window(_name)
{
}

WProperties::~WProperties()
{
}

void WProperties::SetInspectedObject(GameObject* _object)
{
	selectedObject = _object;
}

void WProperties::Draw()
{
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	if (selectedObject)
	{
		// GameObject Options
		strcpy_s(RenameBuf, IM_ARRAYSIZE(RenameBuf), (char*)selectedObject->GetName().c_str());
		ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted("Name:"); ImGui::SameLine();
		if (ImGui::InputText("##NameDisplay", RenameBuf, IM_ARRAYSIZE(RenameBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo, (ImGuiInputTextCallback)0, (void*)this))
		{
			if (strlen(RenameBuf) > 0)
				selectedObject->ChangeName(RenameBuf);
		}

		// Components
		for (std::vector<Component*>::iterator it = selectedObject->GetComponents().begin(), end = selectedObject->GetComponents().end(); it != end; ++it)
		{
			DrawComponentHeader((*it));
		}

		// New Components
		ImGui::Spacing(); ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing(); ImGui::Spacing();
		if (ImGui::Button("Add New Component", ImVec2(ImGui::GetContentRegionAvail().x, 25))) { ImGui::OpenPopup("AddComponentPopup"); }
		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			if (ImGui::MenuItem("Transform"))
			{
				if (!selectedObject->GetTransform())
					selectedObject->AddComponent(TRANSFORM);
			}
			if (ImGui::MenuItem("Mesh"))
			{
				if (!selectedObject->GetComponent<CMesh>())
					selectedObject->AddComponent(MESH);
			}
			if (ImGui::MenuItem("Material"))
			{
				if (!selectedObject->GetComponent<CMaterial>())
					selectedObject->AddComponent(MATERIAL);
			}

			// TODO: Generate opening a window on already existing component creation
			/*if(false)
				ImGui::OpenPopup("Delete?");
			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("A component of this type\ncannot be instanced twice\nin the same Game Object!\n\n");
				ImGui::Separator();
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}*/
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void WProperties::DrawComponentHeader(Component* _component)
{
	std::string name;
	switch (_component->GetType())
	{
	case TRANSFORM:
		name = "Transform"; break;
	case MESH:
		name = "Mesh"; break;
	case MATERIAL:
		name = "Material"; break;
	}

	bool headerOpen = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
	ImGui::SameLine();
	if (ImGui::GetWindowWidth() > 170)
		ImGui::Indent(ImGui::GetWindowWidth() - 85);
	if (ImGui::Button(("Options##" + name).c_str()))
	{
		ImGui::OpenPopup(("Component Options##" + name).c_str());
	}
	if (ImGui::GetWindowWidth() > 170)
		ImGui::Unindent(ImGui::GetWindowWidth() - 85);

	if (headerOpen)
	{
		switch (_component->GetType())
		{
		case TRANSFORM:
			DrawTransformationBody();
			break;
		case MESH:
			DrawMeshBody((CMesh*)_component);
			break;
		case MATERIAL:
			DrawMaterialBody((CMaterial*)_component);
			break;
		}
	}
	if (ImGui::BeginPopup(("Component Options##" + name).c_str()))
	{
		if (ImGui::MenuItem("Remove Component"))
		{
			toDelete = _component;
		}
		ImGui::EndPopup();
	}
}

void WProperties::DrawTransformationBody()
{
	CTransform* transform = selectedObject->GetTransform();
	float3 scale = transform->GetScale();
	float3 rotation = transform->GetRotation();
	float3 position = transform->GetPos();
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };

	ImGui::PushItemWidth(70.f);
	bool modified = false;
	ImGui::TextUnformatted("Position");
	if (ImGui::SliderFloat("X##1", &position.x, -50.0f, 50.0f, "%.2f")) modified = true;
	ImGui::SameLine(); if (ImGui::SliderFloat("Y##1", &position.y, -50.0f, 50.0f, "%.2f")) modified = true;
	ImGui::SameLine(); if (ImGui::SliderFloat("Z##1", &position.z, -50.0f, 50.0f, "%.2f")) modified = true;

	ImGui::TextUnformatted("Rotation");
	if (ImGui::SliderAngle("X##2", &rotation.x)) modified = true;
	ImGui::SameLine(); if (ImGui::SliderAngle("Y##2", &rotation.y)) modified = true;
	ImGui::SameLine(); if (ImGui::SliderAngle("Z##2", &rotation.z)) modified = true;

	ImGui::TextUnformatted("Scale");
	if (ImGui::SliderFloat("X##3", &scale.x, 0.0f, 10.0f, "%.2f")) modified = true;
	ImGui::SameLine(); if (ImGui::SliderFloat("Y##3", &scale.y, 0.0f, 10.0f, "%.2f")) modified = true;
	ImGui::SameLine(); if (ImGui::SliderFloat("Z##3", &scale.z, 0.0f, 10.0f, "%.2f")) modified = true;

	if (modified)
	{
		selectedObject->SetTransform(scale, rotation, position);
		selectedObject->UpdateOBB();
	}
}

void WProperties::DrawMeshBody(CMesh* mesh)
{
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	ImGui::TextUnformatted("Num vertices:"); ImGui::SameLine();
	ImGui::TextColored(color, "%d", mesh->numVertices);
	ImGui::TextUnformatted("Num indices:"); ImGui::SameLine();
	ImGui::TextColored(color, "%d", mesh->numIndices);
}

// Texture
// TODO: should this go somewhere else?
const char* wrap[] = { "Repeat", "Clamp", "Clamp to border", "Mirrored Repeat" };
const char* filterm[] = { "Linear, Mipmap linear", "Linear, Mipmap nearest", "Nearest, Mipmap linear", "Nearest, Mipmap nearest" };
const char* filterM[] = { "Linear", "Nearest" };
void WProperties::DrawMaterialBody(CMaterial* material)
{
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	// TODO: Button to add textures inside this component
	if (ImGui::BeginTabBar("Textures"))
	{
		// TODO: remove this texture from this material
		// TODO: select another file for this texture (and drag and drop?)
		std::string label;
		for (unsigned int i = 0; i < material->textures.size(); ++i)
		{
			label = "Texture " + std::to_string(i);
			ImGui::PushItemWidth(100);
			if (ImGui::BeginTabItem(label.c_str()))
			{
				glBindTexture(GL_TEXTURE_2D, material->textures[i]->id);
				// Texture size
				int w, h;
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
				ImGui::TextUnformatted("Width:");
				ImGui::SameLine();
				ImGui::TextColored(color, "%d", w);
				ImGui::TextUnformatted("Height:");
				ImGui::SameLine();
				ImGui::TextColored(color, "%d", h);

				auto it = std::find(wrapmode.begin(), wrapmode.end(), material->textures[i]->wraps);
				int indexWS = it - wrapmode.begin();
				if (ImGui::Combo("Wrap (S dir)", &indexWS, wrap, IM_ARRAYSIZE(wrap)))
					material->textures[i]->wraps = wrapmode[indexWS];

				it = std::find(wrapmode.begin(), wrapmode.end(), material->textures[i]->wrapt);
				int indexWT = it - wrapmode.begin();
				if (ImGui::Combo("Wrap (T dir)", &indexWT, wrap, IM_ARRAYSIZE(wrap)))
					material->textures[i]->wrapt = wrapmode[indexWT];

				it = std::find(filtermode.begin(), filtermode.end() - 2, material->textures[i]->minfilter);
				int indexFm = it - filtermode.begin();
				if (ImGui::Combo("Minification", &indexFm, filterm, IM_ARRAYSIZE(filterm)))
					material->textures[i]->minfilter = filtermode[indexFm];

				it = std::find(filtermode.end() - 2, filtermode.end(), material->textures[i]->magfilter);
				int indexFM = it - (filtermode.end() - 2);
				if (ImGui::Combo("Magnification", &indexFM, filterM, IM_ARRAYSIZE(filterM)))
					material->textures[i]->magfilter = filtermode[4 + indexFM];
				ImGui::Separator();

				ImTextureID texid = (ImTextureID)material->textures[i]->id;
				float sizeX = ImGui::GetWindowSize().x > 350.f ? 350.f : ImGui::GetWindowSize().x;
				ImGui::Image(texid, ImVec2(sizeX, sizeX * h / (float)w));
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}