#pragma once
#include "Globals.h"
#include "Module.h"
#include <vector>
#include <string>

struct Texture {
	unsigned int id;	// identifier
	std::string type;	// type of texture
	std::string path;	// path to the file on your drive
	int wraps;			// ??
	int wrapt;			// ??
	int minfilter;		// ??
	int magfilter;		// ??

};

class ModuleTexture : public Module
{
public:

	bool mipmap;									// ??
	bool force_flip;								// ??


public:
	ModuleTexture();								// Constructor
	~ModuleTexture();								// Destructor

	//  ----- Module Functions ----- //
	bool Init() override;							// Initialise ModuleWindow
	bool CleanUp() override;						// Clean memory allocated by this Module

	unsigned int LoadTexture(std::string path);		// Loads an image file and attaches it to a GL_TEXTURE_2D

};