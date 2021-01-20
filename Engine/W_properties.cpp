#include "W_properties.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleSceneManager.h"
#include "ModuleFilesystem.h"
#include "GameObject.h"
#include "ImporterMaterial.h"
#include "ImporterMesh.h"
#include "CTransform.h"
#include "CMesh.h"
#include "CMaterial.h"
#include "CCamera.h"
#include "CLight.h"
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

	if (App->sceneMng->GetSelectedGO())
	{
		GameObject* selectedObject = App->sceneMng->GetSelectedGO();
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
				selectedObject->AddComponent(TRANSFORM);
			}
			if (ImGui::MenuItem("Mesh"))
			{
				int size;
				std::string fn = App->filesys->OpenDialog("Mesh Files\0*.mesh\0", ".\\Library\\Meshes\\", &size);
				if (!fn.empty())
				{
					selectedObject->AddComponent(MESH);
					ImporterMesh::Load(fn.c_str(), selectedObject, size);
					selectedObject->SetProgram(App->sceneMng->GetProgram());
					App->camera->cullingCamera->PerformFrustumCulling();
				}
			}
			if (ImGui::MenuItem("Material"))
			{
				int size;
				std::string fn = App->filesys->OpenDialog("Material Files\0*.material\0", ".\\Library\\Materials\\", &size);
				if (!fn.empty())
				{
					selectedObject->AddComponent(MATERIAL); //
					ImporterMaterial::Load(fn, selectedObject->GetComponent<CMaterial>(), size);
				}
			}
			if (ImGui::MenuItem("Camera"))
			{
				selectedObject->AddComponent(CAMERA);
			}
			if (ImGui::MenuItem("Light"))
			{
				selectedObject->AddComponent(LIGHT);
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
	case CAMERA:
		name = "Camera"; break;
	case LIGHT:
		name = "Light"; break;
	}

	bool headerOpen = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);
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
			DrawTransformationBody((CTransform*)_component);
			break;
		case MESH:
			DrawMeshBody((CMesh*)_component);
			break;
		case MATERIAL:
			DrawMaterialBody((CMaterial*)_component);
			break;
		case CAMERA:
			DrawCameraBody((CCamera*)_component);
			break;
		case LIGHT:
			DrawLightBody((CLight*)_component);
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

void WProperties::DrawTransformationBody(CTransform* _transform)
{
	float3 scale = _transform->GetScale();
	float3 rotation = _transform->GetRotation();
	float3 position = _transform->GetPos();
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };

	ImGui::PushItemWidth(70.f);
	bool modified = false;
	ImGui::TextUnformatted("Position");
	if (ImGui::SliderFloat("X##1", &position.x, -10000.0f, 10000.0f, "%.2f")) modified = true;
	ImGui::SameLine(); if (ImGui::SliderFloat("Y##1", &position.y, -10000.0f, 10000.0f, "%.2f")) modified = true;
	ImGui::SameLine(); if (ImGui::SliderFloat("Z##1", &position.z, -10000.0f, 10000.0f, "%.2f")) modified = true;

	ImGui::TextUnformatted("Rotation");
	if (ImGui::SliderAngle("X##2", &rotation.x)) modified = true;
	ImGui::SameLine(); if (ImGui::SliderAngle("Y##2", &rotation.y)) modified = true;
	ImGui::SameLine(); if (ImGui::SliderAngle("Z##2", &rotation.z)) modified = true;

	ImGui::TextUnformatted("Scale");
	if (ImGui::SliderFloat("X##3", &scale.x, 0.0f, 10.0f, "%.3f")) modified = true;
	ImGui::SameLine(); if (ImGui::SliderFloat("Y##3", &scale.y, 0.0f, 10.0f, "%.3f")) modified = true;
	ImGui::SameLine(); if (ImGui::SliderFloat("Z##3", &scale.z, 0.0f, 10.0f, "%.3f")) modified = true;

	if (modified)
	{
		GameObject* selectedObject = App->sceneMng->GetSelectedGO();
		selectedObject->SetTransform(scale, rotation, position);
		selectedObject->UpdateBoundingBoxes();
		selectedObject->UpdateOctreePosition();
		if (selectedObject->GetComponent<CCamera>())
			selectedObject->GetComponent<CCamera>()->UpdateFrustumFromTransform(_transform);
		App->camera->cullingCamera->PerformFrustumCulling();
	}
	ImGui::PopItemWidth();
}

void WProperties::DrawMeshBody(CMesh* _mesh)
{
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	ImGui::TextUnformatted("Num vertices:"); ImGui::SameLine();
	ImGui::TextColored(color, "%d", _mesh->GetNumVertices());
	ImGui::TextUnformatted("Num indices:"); ImGui::SameLine();
	ImGui::TextColored(color, "%d", _mesh->GetNumIndices());
}

// Texture
void WProperties::DrawMaterialBody(CMaterial* _material)
{
	ImGui::PushItemWidth(100);

	// ------ Material base settings ------ //
	// TODO: set ambient color as global, for all gameobjects
	ImGui::ColorEdit3("Set ambient color", &_material->ambient[0], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
	// The following settings are used when material has no diffuse/specular maps
	ImGui::ColorEdit3("Set diffuse color", &_material->diffuse[0], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs); ImGui::SameLine();
	HelpMarker("This color is used when there isn't any diffuse map");
	ImGui::ColorEdit3("Set specular color", &_material->specular[0], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs); ImGui::SameLine();
	HelpMarker("This color is used when there isn't any specular map");
	ImGui::SliderFloat("Shininess", &_material->shininess, 1.0, 300.0f, "%.1f");
	ImGui::InputInt("Shininess in alpha", &_material->shininessAlpha, 1, 1); ImGui::SameLine();
	HelpMarker("Number of the specular map which has the shininess in the alpha channel. If 0, no specular map contains the shininess and the above setting is used instead.");

	if (ImGui::Button("Add Texture")) { ImGui::OpenPopup("AddTexturePopup"); }
	if (ImGui::BeginPopup("AddTexturePopup"))
	{
		std::string type;
		int size;
		std::string fn;
		if (ImGui::MenuItem("Diffuse"))
		{
			type = "diffuse";
			fn = App->filesys->OpenDialog("Texture Files\0*.dds;*.png;*.tif\0", ".\\Library\\Textures\\", &size);
		}
		if (ImGui::MenuItem("Specular"))
		{
			type = "specular";
			fn = App->filesys->OpenDialog("Texture Files\0*.dds;*.png;*.tif\0", ".\\Library\\Textures\\", &size);
		}
		if (!fn.empty())
		{
			Texture* texture = new Texture();
			texture->type = type;
			texture->id = ImporterMaterial::LoadTexture(fn, "./Library/Textures/" + fn.substr(fn.find_last_of('\\')+1, fn.size()), true);
			fn = fn.replace(fn.find_last_of('.'), fn.size(), ".dds");
			texture->path = fn.substr(fn.find_last_of('\\')+1, fn.size());
			texture->wraps = GL_REPEAT;
			texture->wrapt = GL_REPEAT;
			texture->minfilter = GL_LINEAR_MIPMAP_LINEAR;
			texture->magfilter = GL_LINEAR;
			App->filesys->loadedTextures.push_back(texture);
			_material->textures.push_back(App->filesys->loadedTextures[App->filesys->loadedTextures.size() - 1]);
			_material->filesize += 6 * sizeof(int) + sizeof(char) * (texture->path.size() + 1 + texture->type.size() + 1);
		}
		ImGui::EndPopup();
	}
	// -------- Textures settings -------- //
	ImVec4 color = { 0.0f, 0.3f, 1.0f, 1.0f };
	// TODO: Button to add textures inside this component
	if (ImGui::BeginTabBar("Textures"))
	{
		static char* wrap[] = { "Repeat", "Clamp", "Clamp to border", "Mirrored Repeat" };
		static char* filterm[] = { "Linear, Mipmap linear", "Linear, Mipmap nearest", "Nearest, Mipmap linear", "Nearest, Mipmap nearest" };
		static char* filterM[] = { "Linear", "Nearest" };
		// TODO: drag and drop texture
		std::string label;
		for (unsigned int i = 0u; i < _material->textures.size(); ++i)
		{
			label = "Texture " + std::to_string(i);
			if (ImGui::BeginTabItem(label.c_str()))
			{
				if (ImGui::GetWindowWidth() > 170)
					ImGui::Indent(ImGui::GetWindowWidth() - 85);
				bool deleted = ImGui::Button(("Delete"));
				if (ImGui::GetWindowWidth() > 170)
					ImGui::Unindent(ImGui::GetWindowWidth() - 85);
				if (deleted)
				{
					_material->filesize -= 6 * sizeof(int) + sizeof(char) * (_material->textures[i]->path.size() + 1 + _material->textures[i]->type.size() + 1);
					_material->textures.erase(_material->textures.begin() + i);
					--i;
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, _material->textures[i]->id);

					// Texture  type
					ImGui::TextUnformatted("Type:"); ImGui::SameLine();
					ImGui::TextColored(color, "%s", _material->textures[i]->type.c_str());
					// Texture size
					int w, h;
					glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
					glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
					ImGui::TextUnformatted("Width:"); ImGui::SameLine();
					ImGui::TextColored(color, "%d", w);
					ImGui::TextUnformatted("Height:"); ImGui::SameLine();
					ImGui::TextColored(color, "%d", h);

					auto it = std::find(wrapmode.begin(), wrapmode.end(), _material->textures[i]->wraps);
					int indexWS = it - wrapmode.begin();
					if (ImGui::Combo("Wrap (S dir)", &indexWS, wrap, IM_ARRAYSIZE(wrap)))
						_material->textures[i]->wraps = wrapmode[indexWS];

					it = std::find(wrapmode.begin(), wrapmode.end(), _material->textures[i]->wrapt);
					int indexWT = it - wrapmode.begin();
					if (ImGui::Combo("Wrap (T dir)", &indexWT, wrap, IM_ARRAYSIZE(wrap)))
						_material->textures[i]->wrapt = wrapmode[indexWT];

					it = std::find(filtermode.begin(), filtermode.end() - 2, _material->textures[i]->minfilter);
					int indexFm = it - filtermode.begin();
					if (ImGui::Combo("Minification", &indexFm, filterm, IM_ARRAYSIZE(filterm)))
						_material->textures[i]->minfilter = filtermode[indexFm];

					it = std::find(filtermode.end() - 2, filtermode.end(), _material->textures[i]->magfilter);
					int indexFM = it - (filtermode.end() - 2);
					if (ImGui::Combo("Magnification", &indexFM, filterM, IM_ARRAYSIZE(filterM)))
						_material->textures[i]->magfilter = filtermode[4 + indexFM];
					ImGui::Separator();

					ImTextureID texid = (ImTextureID)_material->textures[i]->id;
					float sizeX = ImGui::GetWindowSize().x > 350.f ? 350.f : ImGui::GetWindowSize().x;
					ImGui::Image(texid, ImVec2(sizeX, sizeX * h / (float)w));
				}

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::PopItemWidth();
}

void WProperties::DrawCameraBody(CCamera* _camera)
{
	bool isActive = App->camera->activeCamera == _camera;
	bool isCulling = App->camera->cullingCamera == _camera;

	if (ImGui::Checkbox("Set Camera as Active", &isActive))
	{
		if (isActive)
			App->camera->SetActiveCamera(_camera);
		else
			App->camera->ResetActiveCamera();
	};
	if (ImGui::Checkbox("Set Camera as Culling", &isCulling))
	{
		if (isCulling)
			App->camera->SetCullingCamera(_camera);
		else
			App->camera->ResetCullingCamera();
	};

	ImGui::PushItemWidth(180);
	ImGui::TextUnformatted("Frustum");
	Frustum* frust = _camera->GetFrustum();
	float nearPlane = frust->NearPlaneDistance();
	float farPlane = frust->FarPlaneDistance();
	if (ImGui::InputFloat("Near Plane", &nearPlane, 0.1f, 1.0f, "%.3f")) {
		frust->SetViewPlaneDistances(nearPlane, farPlane);
	}

	if (ImGui::InputFloat("Far Plane", &farPlane, 5.f, 20.0f, "%.3f")) {
		frust->SetViewPlaneDistances(nearPlane, farPlane);
	}

	float VFOV = frust->VerticalFov();
	float ar = frust->AspectRatio();
	if (ImGui::SliderAngle("FOV", &VFOV, 45.f, 110.f)) {
		frust->SetVerticalFovAndAspectRatio(VFOV, ar);
	}
	if (ImGui::SliderFloat("Aspect Ratio", &ar, 0.05f, 24.f)) {
		frust->SetVerticalFovAndAspectRatio(VFOV, ar);	// The aspect ratio is not stored, so when resizing the viewport/window the user can recover the original aspect ratio
	}
	ImGui::PopItemWidth();
}

void WProperties::DrawLightBody(CLight* _light)
{
	const char* items[] = { "Directional Light", "Point Light", "Spot Light" };
	ImGui::PushItemWidth(180);
	ImGui::Combo("Light Type", &_light->GetTypeRef(), items, IM_ARRAYSIZE(items));

	ImGui::Spacing();

	ImGui::ColorEdit3("Light Color", &_light->GetColorRef().x, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
	ImGui::DragFloat("Intensity", &_light->GetIntensityRef(), .02f, 0.0f, FLT_MAX, "%.2f");

	ImGui::PushItemWidth(100);
	switch (_light->GetLType())
	{
	case 0:	// Directional
		break;
	case 1:	// Point
		ImGui::DragFloat("Constant Att.", &_light->GetKcRef(), .02f, 0.0001f, FLT_MAX, "%.2f");
		ImGui::DragFloat("Linear Att.", &_light->GetKlRef(), .02f, 0.0001f, FLT_MAX, "%.2f");
		ImGui::DragFloat("Quadratic Att.", &_light->GetKqRef(), .02f, 0.0001f, FLT_MAX, "%.2f");
		break;
	case 2:	// Spot
		ImGui::DragFloat("Constant Att.", &_light->GetKcRef(), .02f, 0.0001f, FLT_MAX, "%.2f");
		ImGui::DragFloat("Linear Att.", &_light->GetKlRef(), .02f, 0.0001f, FLT_MAX, "%.2f");
		ImGui::DragFloat("Quadratic Att.", &_light->GetKqRef(), .02f, 0.0001f, FLT_MAX, "%.2f");
		ImGui::DragFloat("Inner Angle", &_light->GetInnerAngRef(), .05f, 0.f, 180, "%.2f");
		if (_light->GetOuterAngle() < _light->GetInnerAngle())
			_light->SetOuterAngle(_light->GetInnerAngle());
		ImGui::DragFloat("Outer Angle", &_light->GetOuterAngRef(), .05f, _light->GetInnerAngle(), 180, "%.2f");
		break;
	default:
		ImGui::TextUnformatted("The type of ligt was nos specified!");
		break;
	}
}
