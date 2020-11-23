#include "Model.h"
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Leaks.h"

Model::Model(std::string const& path)
{
	Load(path);
}

Model::~Model()
{
	for (std::vector<Mesh*>::iterator it = meshes.begin(), end = meshes.end(); it != end; ++it) {
		(*it)->CleanUp();
		delete* it;
	}
	for (std::vector<Texture>::iterator it = loaded_textures.begin(), end = loaded_textures.end(); it != end; ++it)
		glDeleteTextures(1, &(it->id));
}

void Model::Draw(unsigned int program)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i]->Draw(program, textures);
	}
}

void Model::Load(std::string const& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		LOG("Error loading %s: %s", path, importer.GetErrorString());
	}
	else
	{
		directory = path.substr(0, path.find_last_of('/')).size() == path.size() ? 
			path.substr(0, path.find_last_of('\\')) : path.substr(0, path.find_last_of('/'));
		LoadTextures(scene);
		LoadMeshes(scene);
	}

}

void Model::LoadMeshes(const aiScene* scene)
{
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		meshes.push_back(new Mesh(scene->mMeshes[i], scene->mMeshes[i]->mMaterialIndex));
	}
}

void Model::LoadTextures(const aiScene* scene)
{
	aiString file;

	textures.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
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
			if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
				Texture texture;
				texture.id = App->textureLoader->LoadTexture(directory + '/' + file.C_Str());
				texture.type = "texture_diffuse";
				texture.path = file.C_Str();
				textures.push_back(texture);
				loaded_textures.push_back(texture);
			}
		}
	}

}
