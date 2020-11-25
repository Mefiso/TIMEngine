#pragma once
#include "Window.h"
#include "ModuleTexture.h"
#include <Math/float4x4.h>

class Model;
class Mesh;

class WProperties : public Window
{
private:
	// Should it be a pointer to a vector?
	std::vector<Texture*> selected_textures;
	float3 scale;
	float3 rotation;
	float3 translation;
	std::vector<Mesh*> selected_meshes;
public:
	WProperties(std::string name, int ID);
	~WProperties();

	void Draw();
	void SelectPropertiesFromModel(Model* model);
};