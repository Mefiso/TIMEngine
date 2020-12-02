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
	float3 scale = float3::zero;
	float3 rotation = float3::zero;
	float3 translation = float3::zero;
	std::vector<Mesh*> selected_meshes;
public:
	WProperties(std::string name);
	~WProperties();

	void Draw();
	void SelectPropertiesFromModel(const Model* model);
private:
	void TransformationHeader() const;
	void GeometryHeader() const;
	void TexturesHeader() const;
};