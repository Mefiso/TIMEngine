#pragma once
#include "Window.h"
#include "ModuleTexture.h"

class Model;

class WProperties : public Window
{
private:
	// Should it be a pointer to a vector?
	std::vector<Texture*> selected_textures;
public:
	WProperties(std::string name, int ID);
	~WProperties();

	void Draw();
	void SelectTextures(Model* model);
};