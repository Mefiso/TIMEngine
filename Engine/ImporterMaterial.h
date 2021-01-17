#pragma once
#include "CMaterial.h"
#include "Application.h"
#include "ModuleFilesystem.h"
#include <GL/glew.h>
#include <assimp/material.h>
#include <string>

struct Texture {
	unsigned int	id;			// identifier
	std::string		type;		// type of texture
	std::string		path;		// path to the file on your drive
	int				wraps;		// Identifier of the Horizontal Wrapping of the texture. Select from W_config -> wrap[].
	int				wrapt;		// Identifier of the Vertical Wrapping of the texture. Select from W_config -> wrap[].
	int				minfilter;	// Identifier of the Minification filter applied to the specified Texture. Select from W_config -> filterm[].
	int				magfilter;	// Identifier of the Magnification filter applied to the specified Texture. Select from W_config -> filterM[].
};

class GameObject;
namespace ImporterMaterial
{
	bool			Import(aiMaterial* _material, const std::string& _path, GameObject* _parent);	// Imports a Material with all its textures from Assimp
	bool			Load(std::string _filename, CMaterial* _cmaterial, unsigned int _filesize);		// Loads a Material with all its textures from a file in custom file format
	unsigned int	Save(CMaterial* _material, const char* _filename);								// Saves the info of a Component Material into a file in custom file format

	unsigned int	LoadTexture(std::string _path, std::string _destPath, bool _saveToCustom=false);	// Loads an image file and attaches it to a GL_TEXTURE_2D
	unsigned int	LoadCubemap(std::vector<std::string> _faces);									// Loads a Cubemap Texture from 6 images ('faces') and attaches it to a GL_TEXTURE_CUBE_MAP
	void			SaveTexture(const char* _destPath);												// Saves a texture loaded in DevIL to a new .DDS file

	namespace	// Private functions
	{
		// Iterates over the textures that come from an Assimp material and loads them using LoadTexture
		void LoadMaterialTextures(aiMaterial* _material, aiTextureType _type, const std::string& _name, const std::string& _path, std::vector<Texture*>* _matTextures)
		{
			// Iterates over all material textures of the specified type
			_matTextures->reserve(_material->GetTextureCount(_type));
			for (unsigned int i = 0; i < _material->GetTextureCount(_type); ++i)
			{
				aiString file;
				if (_material->GetTexture(_type, i, &file) == AI_SUCCESS)
				{
					// Checks if texture is already loaded in scene. If so, skips loading texture from file.
					bool skip = false;
					std::string ff = file.C_Str();
					for (unsigned j = 0; j < App->filesys->loadedTextures.size(); ++j)
					{
						std::string pp = App->filesys->loadedTextures[j]->path;
						if (ff.substr(0, ff.find_last_of('.')).compare(pp.substr(0, pp.find_last_of('.'))) == 0)
						{
							_matTextures->push_back(App->filesys->loadedTextures[j]);
							skip = true;
							break;
						}
					}
					// If the texture has never been loaded, tries to load it from file
					if (!skip)
					{
						bool texFound = true;
						Texture* texture = new Texture();
						std::string destPath = "./Library/Textures/";
						destPath.append(_material->GetName().C_Str());
						destPath.append(_name + std::to_string(i));
						destPath.append(".dds");
						LOG("[info] Trying to find texture on the path specified by the fbx: %s", file.C_Str());
						texture->id = LoadTexture(file.C_Str(), destPath, true);
						//Save(destPath.c_str());
						if (!texture->id) {
							LOG("[info] Failed to load textures.");
							LOG("[info] Trying to find texture on the same folder as fbx: %s", (_path + '/' + file.C_Str()).c_str());
							texture->id = LoadTexture(_path + '/' + file.C_Str(), destPath, true);
							//Save(destPath.c_str());

							if (!texture->id) {
								LOG("[info] Failed to load textures.");
								LOG("[info] Trying to find texture on the textures folder.");
								texture->id = LoadTexture(std::string("./resources/textures/") + file.C_Str(), destPath, true);
								//Save(destPath.c_str());
								if (!texture->id) {
									LOG("[error] Texture %s not found.", file.C_Str());
									texFound = false;
								}
							}
						}
						// Add Texture to loaded textures vector
						if (texFound) {
							LOG("[info] Texture loaded.");
							texture->path = destPath.substr(19, destPath.size());
							texture->type = _name;
							texture->wraps = GL_REPEAT;
							texture->wrapt = GL_REPEAT;
							texture->minfilter = GL_LINEAR_MIPMAP_LINEAR;
							texture->magfilter = GL_LINEAR;
							App->filesys->loadedTextures.push_back(texture);
							_matTextures->push_back(App->filesys->loadedTextures[App->filesys->loadedTextures.size() - 1]);
						}
					}
				}
			}
		}
		// ------------------------------------------------------------------------------------- //
	}
};

