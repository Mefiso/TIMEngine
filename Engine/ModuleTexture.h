#pragma once
#include "Globals.h"
#include "Module.h"
#include <vector>
#include <string>

struct Texture {
	unsigned int id;	// identifier
	std::string type;	// type of texture
	std::string path;	// path to the file on your drive
	int wraps;			// Identifier of the Horizontal Wrapping of the texture. Select from W_config -> wrap[].
	int wrapt;			// Identifier of the Vertical Wrapping of the texture. Select from W_config -> wrap[].
	int minfilter;		// Identifier of the Minification filter applied to the specified Texture. Mipmap option only applies if the Mipmap generation is activated. Select from W_config -> filterm[].
	int magfilter;		// Identifier of the Magnification filter applied to the specified Texture. Select from W_config -> filterM[].

};

class ModuleTexture : public Module
{
public:

	bool mipmap;									// Activates Mipmap generation for the following loaded models
	bool force_flip;								// Activates the flipping of the Difuse Texture loaded with the next model


public:
	ModuleTexture();								// Constructor
	~ModuleTexture();								// Destructor

	//  ----- Module Functions ----- //
	bool Init() override;							// Initialise ModuleWindow
	bool CleanUp() override;						// Clean memory allocated by this Module

	unsigned int LoadTexture(std::string path);		// Loads an image file and attaches it to a GL_TEXTURE_2D

};