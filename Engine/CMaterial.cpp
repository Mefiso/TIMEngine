#include "CMaterial.h"

#include "GL/glew.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTexture.h"
#include "Leaks.h"

CMaterial::CMaterial(GameObject* _owner, const aiMaterial* material, const std::string& path) : Component(MATERIAL, _owner)
{
	std::vector<Texture*>* diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", path);
	textures.insert(textures.end(), diffuseMaps->begin(), diffuseMaps->end());
	std::vector<Texture*>* specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular", path);
	textures.insert(textures.end(), specularMaps->begin(), specularMaps->end());
	RELEASE(diffuseMaps);
	RELEASE(specularMaps);
}

CMaterial::~CMaterial()
{
	textures.clear();
}

std::vector<Texture*>* CMaterial::LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name, const std::string& path)
{
	std::vector<Texture*>* matTextures = new std::vector<Texture*>();
	matTextures->reserve(material->GetTextureCount(type));
	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString file;
		if (material->GetTexture(type, i, &file) == AI_SUCCESS)
		{
			bool skip = false;
			for (unsigned j = 0; j < App->scene->loadedTextures.size(); ++j)
			{
				if (std::strcmp(App->scene->loadedTextures[j]->path.data(), file.C_Str()) == 0)
				{
					matTextures->push_back(App->scene->loadedTextures[j]);
					skip = true;
					break;
				}
			}
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
				if (texFound) {
					LOG("[info] Texture loaded.");
					texture->path = file.C_Str();
					texture->type = name;
					texture->wraps = GL_REPEAT;
					texture->wrapt = GL_REPEAT;
					texture->minfilter = GL_LINEAR_MIPMAP_LINEAR;
					texture->magfilter = GL_LINEAR;
					App->scene->loadedTextures.push_back(texture);
					matTextures->push_back(App->scene->loadedTextures[App->scene->loadedTextures.size() - 1]);
				}
			}
		}
	}
	return matTextures;
}