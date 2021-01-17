#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>

struct Texture;

class ModuleFilesystem : public Module
{
public:
	std::vector<Texture*> loadedTextures;										// A vector containing All the texures loaded

public:
	ModuleFilesystem();															// Constructor
	~ModuleFilesystem();														// Destructor

	//  ----- Module Functions ----- //
	bool Start() override;														// Load default scene
	bool CleanUp() override;													// Clean memory allocated by this Module

	void DropFile(const std::string& file);										// Called when a Drop File event is recieved. Calls the appropiate importer for the type of file.
	std::string OpenDialog(const char* _filters, const char* dir, int* _size) const;
};