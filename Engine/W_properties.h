#pragma once
#include "Window.h"
#include "ModuleTexture.h"

class WProperties : public Window
{
private:
	std::vector<Texture*> selected_textures;
public:
	WProperties(std::string name, int ID);
	~WProperties();

	void Draw();
	void SelectTextures();
};