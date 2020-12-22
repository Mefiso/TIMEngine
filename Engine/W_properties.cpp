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

void WProperties::Draw()
{
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	if (selectedObject)
	{
		int nrMeshes = 1;
		for (std::vector<Component*>::iterator it = selectedObject->GetComponents().begin(), end = selectedObject->GetComponents().end(); it != end; ++it)
		{
			switch ((*it)->GetType())
			{
			case TRANSFORM:
				DrawTransformationHeader();
				break;
			case MESH:
				DrawMeshHeader((CMesh*)(*it), nrMeshes++);
				break;
			case MATERIAL:
				DrawMaterialHeader((CMaterial*)(*it));
				break;
			}
		}
	}
	ImGui::End();
}

void WProperties::SetInspectedObject(GameObject* _object)
{
	selectedObject = _object;
}

void WProperties::DrawTransformationHeader() const
{
	CTransform* transform = selectedObject->GetTransform();

	float3 scale = transform->GetScale();
	float3 rotation = transform->GetRotation();
	float3 position = transform->GetPos();

	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::PushItemWidth(70.f);
		bool modified = false;
		ImGui::TextUnformatted("Position");
		if (ImGui::SliderFloat("X##1", &position.x, -100.0f , 100.0f, "%.2f")) modified = true;
		ImGui::SameLine(); if (ImGui::SliderFloat("Y##1", &position.x, -100.0f, 100.0f, "%.2f")) modified = true;
		ImGui::SameLine(); if (ImGui::SliderFloat("Z##1", &position.x, -100.0f, 100.0f, "%.2f")) modified = true;

		ImGui::TextUnformatted("Rotation");
		if (ImGui::SliderAngle("X##2", &rotation.x)) modified = true;
		ImGui::SameLine(); if (ImGui::SliderAngle("Y##2", &rotation.y)) modified = true;
		ImGui::SameLine(); if (ImGui::SliderAngle("Z##2", &rotation.z)) modified = true;

		ImGui::TextUnformatted("Scale");
		if (ImGui::SliderFloat("X##3", &scale.x, -10.0f, 10.0f, "%.2f")) modified = true;
		ImGui::SameLine(); if (ImGui::SliderFloat("Y##3", &scale.x, -10.0f, 10.0f, "%.2f")) modified = true;
		ImGui::SameLine(); if (ImGui::SliderFloat("Z##3", &scale.x, -10.0f, 10.0f, "%.2f")) modified = true;

		if (modified)
			selectedObject->SetTransform(scale, rotation, position);
	}
}

void WProperties::DrawMeshHeader(CMesh* mesh, int id) const
{
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	if (ImGui::CollapsingHeader(("Mesh##" + std::to_string(id)).c_str()))
	{
		ImGui::TextUnformatted("Num vertices:"); ImGui::SameLine();
		ImGui::TextColored(color, "%d", mesh->numVertices);
		ImGui::TextUnformatted("Num indices:"); ImGui::SameLine();
		ImGui::TextColored(color, "%d", mesh->numIndices);
	}
}

// Texture
const char* wrap[] = { "Repeat", "Clamp", "Clamp to border", "Mirrored Repeat" };
const char* filterm[] = { "Linear, Mipmap linear", "Linear, Mipmap nearest", "Nearest, Mipmap linear", "Nearest, Mipmap nearest" };
const char* filterM[] = { "Linear", "Nearest" };
void WProperties::DrawMaterialHeader(CMaterial* material) const
{
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	if (ImGui::CollapsingHeader("Material")) {
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
}