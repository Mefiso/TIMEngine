#include "W_properties.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "GL/glew.h"
#include "Leaks.h"

WProperties::WProperties(std::string name) : Window(name)
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
	
	TransformationHeader();
	GeometryHeader();
	TexturesHeader();
	
	ImGui::End();
}

void WProperties::SelectPropertiesFromModel(const Model* model)
{
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
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	if (ImGui::CollapsingHeader("Transformation"))
	{
		ImGui::TextUnformatted("Translation");
		ImGui::TextUnformatted("x:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f", translation.x);
		ImGui::SameLine();
		ImGui::TextUnformatted("y:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f", translation.y);
		ImGui::SameLine();
		ImGui::TextUnformatted("z:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f", translation.z);
		ImGui::Separator();

		ImGui::TextUnformatted("Rotation");
		ImGui::TextUnformatted("X-axis:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f°", rotation.x);
		ImGui::SameLine();
		ImGui::TextUnformatted("Y-axis:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f°", rotation.y);
		ImGui::SameLine();
		ImGui::TextUnformatted("Z-axis:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f°", rotation.z);
		ImGui::Separator();

		ImGui::TextUnformatted("Scale");
		ImGui::TextUnformatted("x:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f", scale.x);
		ImGui::SameLine();
		ImGui::TextUnformatted("y:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f", scale.y);
		ImGui::SameLine();
		ImGui::TextUnformatted("z:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%.1f", scale.z);
		ImGui::Separator();
	}
}

void WProperties::GeometryHeader() const
{
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	if (ImGui::CollapsingHeader("Geometry"))
	{
		ImGui::TextUnformatted("Num meshes:");
		ImGui::SameLine();
		//ImGui::TextColored(color, "%d", selected_meshes.size());
		ImGui::TextUnformatted("Num textures:");
		ImGui::SameLine();
		ImGui::TextColored(color, "%d", selected_textures.size());

		/*for (unsigned int i = 0; i < selected_meshes.size(); ++i)
		{
			ImGui::Separator();
			ImGui::Text("Mesh %d", i);
			ImGui::Text("Num vertices: %d", selected_meshes[i]->num_vertices);
			ImGui::Text("Num triangles: %d", selected_meshes[i]->num_indices / 3);
		}*/
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
			for (unsigned int i = 0; i < selected_textures.size(); ++i)
			{
				label = "Texture " + std::to_string(i);
				if (ImGui::BeginTabItem(label.c_str()))
				{

					glBindTexture(GL_TEXTURE_2D, selected_textures[i]->id);
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

					ImTextureID texid = (ImTextureID)selected_textures[i]->id;
					float sizeX = ImGui::GetWindowSize().x > 350.f ? 350.f : ImGui::GetWindowSize().x;
					ImGui::Image(texid, ImVec2(sizeX, sizeX * h / (float)w));
					ImGui::EndTabItem();
				}

			}
			ImGui::EndTabBar();
		}
	}
}
