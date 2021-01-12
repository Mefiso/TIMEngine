#pragma once
#include "CMaterial.h"
#include "Application.h"
#include "ModuleFilesystem.h"
#include "GL/glew.h"
#include <assimp/material.h>
#include <string>

struct Texture {
	unsigned int id;	// identifier
	std::string type;	// type of texture
	std::string path;	// path to the file on your drive
	int wraps;			// Identifier of the Horizontal Wrapping of the texture. Select from W_config -> wrap[].
	int wrapt;			// Identifier of the Vertical Wrapping of the texture. Select from W_config -> wrap[].
	int minfilter;		// Identifier of the Minification filter applied to the specified Texture. Select from W_config -> filterm[].
	int magfilter;		// Identifier of the Magnification filter applied to the specified Texture. Select from W_config -> filterM[].
};

class GameObject;
namespace ImporterMaterial
{
	bool Import(const aiMaterial* _material, const std::string& _path, GameObject* _parent);
	unsigned int Save(const CMaterial* ourMaterial, const char* _filename);
	void Load(const char* fileBuffer, CMaterial* ourMaterial);
	//void LoadCheckers(Material* ourMaterial);

	unsigned int LoadTexture(std::string path);					// Loads an image file and attaches it to a GL_TEXTURE_2D
	unsigned int LoadCubemap(std::vector<std::string> faces);	// Loads a Cubemap Texture from 6 images ('faces') and attaches it to a GL_TEXTURE_CUBE_MAP

	namespace
	{
		void LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name, const std::string& path, std::vector<Texture*>* _matTextures)
		{
			// Iterates over all material textures of the specified type
			_matTextures->reserve(material->GetTextureCount(type));
			for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
			{
				aiString file;
				if (material->GetTexture(type, i, &file) == AI_SUCCESS)
				{
					// Checks if texture is already loaded in scene. If so, skips loading texture from file.
					bool skip = false;
					for (unsigned j = 0; j < App->filesys->loadedTextures.size(); ++j)
					{
						if (std::strcmp(App->filesys->loadedTextures[j]->path.data(), file.C_Str()) == 0)
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
						LOG("[info] Trying to find texture on the path specified by the fbx: %s", file.C_Str());
						texture->id = LoadTexture(file.C_Str());
						if (!texture->id) {
							LOG("[info] Failed to load textures.");
							LOG("[info] Trying to find texture on the same folder as fbx: %s", (path + '/' + file.C_Str()).c_str());
							texture->id = LoadTexture(path + '/' + file.C_Str());

							if (!texture->id) {
								LOG("[info] Failed to load textures.");
								LOG("[info] Trying to find texture on the textures folder.");
								texture->id = LoadTexture(std::string("./resources/textures/") + file.C_Str());
								if (!texture->id) {
									LOG("[error] Texture %s not found.", file.C_Str());
									texFound = false;
								}
							}
						}
						// Add Texture to loaded textures vector
						if (texFound) {
							LOG("[info] Texture loaded.");
							texture->path = file.C_Str();
							texture->type = name;
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
	}
};
