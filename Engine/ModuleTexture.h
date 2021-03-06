#pragma once
#include "Globals.h"
#include "Module.h"
#include <vector>
#include <string>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
	int wraps;
	int wrapt;
	int minfilter;
	int magfilter;

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
};