#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Leaks.h"

Model::Model(std::string const& path)
{
	Load(path);
}

void Model::Load(std::string const& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		LOG("Error loading %s: %s", file, importer.GetErrorString());
	}
	else
	{
		LoadTextures(scene);
		LoadMeshes(scene);
	}
}

void Model::LoadMeshes(const aiScene* scene)
{

}

void Model::LoadTextures(const aiScene* scene)
{
	aiString file;

	textures.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
		{
			bool skip = false;
			for (unsigned j = 0; j < loaded_textures.size(); ++j)
			{
				if (std::strcmp(loaded_textures[j].path.data(), file.C_Str()) == 0)
				{
					textures.push_back(loaded_textures[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture texture;
				texture.id = App->textureLoader->LoadTexture(file.C_Str());
				texture.type = "texture_diffuse";
				texture.path = file.C_Str();
				textures.push_back(texture);
				loaded_textures.push_back(texture);
			}
		}
	}

}
