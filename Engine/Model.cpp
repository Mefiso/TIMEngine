#include "Model.h"
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Leaks.h"

/*void AssimpLog(const char* msg, char* user) {
	if (msg) 
		LOG("[info] Assimp Log: %s", msg);
}*/

Model::Model()
{
	//stream.callback = AssimpLog;
	//aiAttachLogStream(&stream);
}

Model::~Model()
{
	CleanUp();
}

void Model::CleanUp()
{
	for (std::vector<Mesh*>::iterator it = meshes.begin(), end = meshes.end(); it != end; ++it) {
		(*it)->CleanUp();
		delete* it;
	}
	meshes.clear();
	for (std::vector<Texture>::iterator it = loaded_textures.begin(), end = loaded_textures.end(); it != end; ++it)
		glDeleteTextures(1, &(it->id));
	textures.clear();
}

void Model::Draw(unsigned int program)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i]->Draw(program, textures, this->transformation);
	}
}

void Model::Load(std::string const& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		LOG("[error] Error loading %s: %s", path, importer.GetErrorString());
	}
	else
	{
		if (!meshes.empty() || !textures.empty())
			CleanUp();
		directory = path.substr(0, path.find_last_of('/')).size() == path.size() ? 
			path.substr(0, path.find_last_of('\\')) : path.substr(0, path.find_last_of('/'));
		LoadTextures(scene);
		LoadMeshes(scene);

		// Enclosing sphere
		DefineSphere(scene);
	}

}

void Model::ReloadTexture(const char* path)
{
	for (std::vector<Texture*>::iterator it = textures.begin(), end = textures.end(); it != end; ++it) {
		bool skip = false;
		for (unsigned j = 0; j < loaded_textures.size(); ++j)
		{
			if (std::strcmp(loaded_textures[j].path.data(), path) == 0)
			{
				(*it) = &loaded_textures[j];
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = ModuleTexture::LoadTexture(path);
			if (texture.id) {
				texture.type = "diffuse";
				texture.path = path;
				texture.wraps = GL_REPEAT;
				texture.wrapt = GL_REPEAT;
				texture.minfilter = GL_LINEAR_MIPMAP_LINEAR;
				texture.magfilter = GL_LINEAR;
				loaded_textures.push_back(texture);
				(*it) = &loaded_textures[loaded_textures.size() - 1];
			}
		}
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
				textures.push_back(&loaded_textures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
				Texture texture;
				LOG("[info] Trying to find texture on the path specified by the fbx: %s", file.C_Str());
				texture.id = ModuleTexture::LoadTexture(file.C_Str());
				if (!texture.id) {
					LOG("[info] Failed to load textures.");
					LOG("[info] Trying to find texture on the same folder as fbx: %s", (directory + '/' + file.C_Str()).c_str());
					texture.id = ModuleTexture::LoadTexture(directory + '/' + file.C_Str());
					
					if (!texture.id) {
						LOG("[info] Failed to load textures.");
						LOG("[info] Trying to find texture on the textures folder.");
						texture.id = ModuleTexture::LoadTexture(std::string("./resources/textures/") + file.C_Str());
						if (!texture.id) {
							LOG("[error] Texture not found.");
							return;
						}
						else {
							texture.path = std::string("./resources/textures/") + file.C_Str();
						}
					}
					else {
						texture.path = directory + '/' + file.C_Str();
					}
				}
				else {
					texture.path = file.C_Str();
				}
				LOG("[info] Texture loaded.");
				texture.type = "diffuse";
				texture.wraps = GL_REPEAT;
				texture.wrapt = GL_REPEAT;
				texture.minfilter = GL_LINEAR_MIPMAP_LINEAR;
				texture.magfilter = GL_LINEAR;
				loaded_textures.push_back(texture);
				textures.push_back(&loaded_textures[loaded_textures.size() - 1]);
				
			}
		}
	}

}

void Model::DefineSphere(const aiScene* scene)
{
	int total_vertices = 0;
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		total_vertices += meshes[i]->num_vertices;
	}
	std::vector<float3> points;
	points.reserve(total_vertices);

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		for (unsigned int j = 0; j < scene->mMeshes[i]->mNumVertices; ++j)
		{
			points.push_back(float3(scene->mMeshes[i]->mVertices[j].x, 
				scene->mMeshes[i]->mVertices[j].y, 
				scene->mMeshes[i]->mVertices[j].z ));
		}
	}

	enclosingSphere = Sphere::FastEnclosingSphere(points.data(), total_vertices);
}


