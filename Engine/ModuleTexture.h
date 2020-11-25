#pragma once
#include "Globals.h"
#include "Module.h"
#include <vector>
#include <string>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;

};

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	bool CleanUp();

	unsigned int LoadTexture(std::string path);
public:
	bool mipmap;
	bool force_flip;
	int wrap_s;
	int wrap_t;
	int filter_mag;
	int filter_min;
};