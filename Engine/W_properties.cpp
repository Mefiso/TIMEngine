#include "W_properties.h"
#include "GL/glew.h"
#include "Model.h"
#include "Leaks.h"

WProperties::WProperties(std::string name, int ID) : Window(name, ID)
{

}

WProperties::~WProperties()
{
}

void WProperties::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(420, 500), ImGuiCond_Once);
	if (!ImGui::Begin(name.c_str(), &active))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Texture")) {
		// This is read-only the texture properties, but in would be interesting to be able to change them
		if (ImGui::BeginTabBar("Textures"))
		{
			std::string label;
			for (unsigned int i = 0; i < selected_textures.size(); i++)
			{
				label = "Texture " + std::to_string(i);
				if (ImGui::BeginTabItem(label.c_str()))
				{
					ImVec4 color = { 0.0, 0.3, 1.0, 1.0 };
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

					// Texture wrap and filter
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
					ImTextureID texid = (ImTextureID) selected_textures[i]->id;
					ImGui::Image(texid, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().x*h/(float)w));
					ImGui::EndTabItem();
				}

			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void WProperties::SelectTextures(Model* model)
{
	selected_textures = model->textures;
}
