#include "W_properties.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "CTransform.h"
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
		for (std::vector<Component*>::iterator it = selectedObject->GetComponents().begin(), end = selectedObject->GetComponents().end(); it != end; ++it)
		{
			switch ((*it)->GetType())
			{
			case TRANSFORM:
				TransformationHeader();
				break;
			case MESH:
				MeshHeader((CMesh*)(*it));
				break;
				//TexturesHeader();
			}
		}
	}
	ImGui::End();
}

void WProperties::SetInspectedObject(GameObject* _object)
{
	selectedObject = _object;
	/*selected_textures = model->textures;
	scale.x = model->transformation.Col3(0).Length();
	scale.y = model->transformation.Col3(1).Length();
	scale.z = model->transformation.Col3(2).Length();

	rotation.x = RadToDeg(acos(model->transformation.Col3(0).Normalized().Dot(float3::unitX)));
	rotation.y = RadToDeg(acos(model->transformation.Col3(1).Normalized().Dot(float3::unitY)));
	rotation.z = RadToDeg(acos(model->transformation.Col3(2).Normalized().Dot(float3::unitZ)));

	translation = model->transformation.Col3(3);

	selected_meshes = model->meshes;*/
}

void WProperties::TransformationHeader() const
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
		if (ImGui::DragFloat("X##1", &position.x, 0.1f, -FLT_MAX, FLT_MAX, "%.2f")) modified = true;
		ImGui::SameLine(); if (ImGui::DragFloat("Y##1", &position.y, 0.1f, -FLT_MAX, FLT_MAX, "%.2f")) modified = true;
		ImGui::SameLine(); if (ImGui::DragFloat("Z##1", &position.z, 0.1f, -FLT_MAX, FLT_MAX, "%.2f")) modified = true;

		ImGui::TextUnformatted("Rotation");
		if (ImGui::SliderAngle("X##2", &rotation.x)) modified = true;
		ImGui::SameLine(); if (ImGui::SliderAngle("Y##2", &rotation.y)) modified = true;
		ImGui::SameLine(); if (ImGui::SliderAngle("Z##2", &rotation.z)) modified = true;

		ImGui::TextUnformatted("Scale");
		if (ImGui::DragFloat("X##3", &scale.x, 0.1f, -FLT_MAX, FLT_MAX, "%.2f")) modified = true;
		ImGui::SameLine(); if (ImGui::DragFloat("Y##3", &scale.y, 0.1f, -FLT_MAX, FLT_MAX, "%.2f")) modified = true;
		ImGui::SameLine(); if (ImGui::DragFloat("Z##3", &scale.z, 0.1f, -FLT_MAX, FLT_MAX, "%.2f")) modified = true;

		if (modified)
			selectedObject->SetTransform(scale, rotation, position);

	}
}

void WProperties::MeshHeader(CMesh* mesh) const
{
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	if (ImGui::CollapsingHeader("Mesh"))
	{
		ImGui::TextUnformatted("Num vertices:"); ImGui::SameLine();
		ImGui::TextColored(color, "%d", mesh->numVertices);
		ImGui::TextUnformatted("Num indices:"); ImGui::SameLine();
		ImGui::TextColored(color, "%d", mesh->numIndices);
	}
}

void WProperties::TexturesHeader() const
{
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	if (ImGui::CollapsingHeader("Textures")) {
		// This is read-only the texture properties, but in would be interesting to be able to change them
		if (ImGui::BeginTabBar("Textures"))
		{
			std::string label;
			for (unsigned int i = 0; i < selectedTextures.size(); ++i)
			{
				label = "Texture " + std::to_string(i);
				if (ImGui::BeginTabItem(label.c_str()))
				{
					glBindTexture(GL_TEXTURE_2D, selectedTextures[i]->id);
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

					// Texture wrap and filter. This is repeated in Texture settings, so it can be deleted.
					GLint wraps, wrapt, filtermag, filtermin;
					glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wraps);
					glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapt);
					glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &filtermag);
					glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &filtermin);
					ImGui::TextUnformatted("Wrap s, t:");
					ImGui::SameLine();
					switch (wraps)
					{
					case GL_CLAMP_TO_BORDER:
						ImGui::TextColored(color, "Clamp to border,");
						break;
					case GL_CLAMP:
						ImGui::TextColored(color, "Clamp,");
						break;
					case GL_REPEAT:
						ImGui::TextColored(color, "Repeat,");
						break;
					case GL_MIRRORED_REPEAT:
						ImGui::TextColored(color, "Mirrored repeat,");
						break;
					}
					ImGui::SameLine();
					switch (wrapt)
					{
					case GL_CLAMP_TO_BORDER:
						ImGui::TextColored(color, "Clamp to border");
						break;
					case GL_CLAMP:
						ImGui::TextColored(color, "Clamp");
						break;
					case GL_REPEAT:
						ImGui::TextColored(color, "Repeat");
						break;
					case GL_MIRRORED_REPEAT:
						ImGui::TextColored(color, "Mirrored repeat");
						break;
					}
					ImGui::TextUnformatted("Filter mag, min:");
					ImGui::SameLine();
					switch (filtermag)
					{
					case GL_NEAREST:
						ImGui::TextColored(color, "Nearest,");
						break;
					case GL_LINEAR:
						ImGui::TextColored(color, "Linear,");
						break;
					}
					ImGui::SameLine();
					switch (filtermin)
					{
					case GL_NEAREST:
						ImGui::TextColored(color, "Nearest");
						break;
					case GL_LINEAR:
						ImGui::TextColored(color, "Linear");
						break;
					case GL_NEAREST_MIPMAP_NEAREST:
						ImGui::TextColored(color, "Nearest, Nearest mipmap");
						break;
					case GL_NEAREST_MIPMAP_LINEAR:
						ImGui::TextColored(color, "Nearest, Linear mipmap");
						break;
					case GL_LINEAR_MIPMAP_NEAREST:
						ImGui::TextColored(color, "Linear, Nearest mipmap");
						break;
					case GL_LINEAR_MIPMAP_LINEAR:
						ImGui::TextColored(color, "Linear, Linear mipmap");
						break;
					}
					ImGui::Separator();

					ImTextureID texid = (ImTextureID)selectedTextures[i]->id;
					float sizeX = ImGui::GetWindowSize().x > 350.f ? 350.f : ImGui::GetWindowSize().x;
					ImGui::Image(texid, ImVec2(sizeX, sizeX * h / (float)w));
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
}