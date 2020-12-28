#include "CMaterial.h"

#include "GL/glew.h"

#include "Application.h"
#include "ModuleSceneLoad.h"
#include "ModuleTexture.h"
#include "Leaks.h"

CMaterial::CMaterial(GameObject* _owner, const aiMaterial* material, const std::string& path) : Component(MATERIAL, _owner)
{
	std::vector<Texture*>* diffuseMaps = new std::vector<Texture*>();
	LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", path, diffuseMaps);
	textures.insert(textures.end(), diffuseMaps->begin(), diffuseMaps->end());
	std::vector<Texture*>* specularMaps = new std::vector<Texture*>();
	LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular", path, specularMaps);
	textures.insert(textures.end(), specularMaps->begin(), specularMaps->end());
	RELEASE(diffuseMaps);
	RELEASE(specularMaps);
}

CMaterial::~CMaterial()
{
	textures.clear();
}

void CMaterial::LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name, const std::string& path, std::vector<Texture*>* _matTextures)
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
			for (unsigned j = 0; j < App->sceneLoad->loadedTextures.size(); ++j)
			{
				if (std::strcmp(App->sceneLoad->loadedTextures[j]->path.data(), file.C_Str()) == 0)
				{
					_matTextures->push_back(App->sceneLoad->loadedTextures[j]);
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
				texture->id = ModuleTexture::LoadTexture(file.C_Str());
				if (!texture->id) {
					LOG("[info] Failed to load textures.");
					LOG("[info] Trying to find texture on the same folder as fbx: %s", (path + '/' + file.C_Str()).c_str());
					texture->id = ModuleTexture::LoadTexture(path + '/' + file.C_Str());

					if (!texture->id) {
						LOG("[info] Failed to load textures.");
						LOG("[info] Trying to find texture on the textures folder.");
						texture->id = ModuleTexture::LoadTexture(std::string("./resources/textures/") + file.C_Str());
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
					App->sceneLoad->loadedTextures.push_back(texture);
					_matTextures->push_back(App->sceneLoad->loadedTextures[App->sceneLoad->loadedTextures.size() - 1]);
				}
			}
		}
	}
}