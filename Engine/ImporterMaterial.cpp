#include "ImporterMaterial.h"
#include "GameObject.h"

#include <IL/il.h>
#include <IL/devil_cpp_wrapper.hpp>


bool ImporterMaterial::Import(aiMaterial* _material, const std::string& _path, GameObject* _parent)
{
	if (_parent->AddComponent(MATERIAL))
	{
		if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
		{
			return false;
		}
		ilInit();
		iluInit();

		CMaterial* cmaterial = _parent->GetComponent<CMaterial>();
		std::vector<Texture*>* diffuseMaps = new std::vector<Texture*>();
		LoadMaterialTextures(_material, aiTextureType_DIFFUSE, "diffuse", _path, diffuseMaps);
		cmaterial->textures.insert(cmaterial->textures.end(), diffuseMaps->begin(), diffuseMaps->end());
		std::vector<Texture*>* specularMaps = new std::vector<Texture*>();
		LoadMaterialTextures(_material, aiTextureType_SPECULAR, "specular", _path, specularMaps);
		cmaterial->textures.insert(cmaterial->textures.end(), specularMaps->begin(), specularMaps->end());
		cmaterial->filename = _material->GetName().C_Str();
		cmaterial->filename.append(".mat");

		RELEASE(diffuseMaps);
		RELEASE(specularMaps);
		ilShutDown();

		return true;
	}
	return false;
}

void ImporterMaterial::SaveTexture(const char* _destPath)
{
	// Open file to write
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _destPath, "wb")) != 0) {
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s", _destPath, strerror(err));
	}
	else {
		ILuint size;
		ILubyte* data;
		ILinfo info;

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);	// To pick a specific DXT compression use
		size = ilSaveL(IL_DDS, nullptr, 0);		// Get the size of the data buffer

		iluGetImageInfo(&info);
		if (IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		if (size > 0)
		{
			char* fileBuffer = nullptr;
			data = new ILubyte[size]; // allocate data buffer
			if (ilSaveL(IL_DDS, data, size) > 0) {
				// Save to buffer with the ilSaveIL function
				fileBuffer = (char*)data;
				fwrite(fileBuffer, sizeof(char), size, f);
				fileBuffer = nullptr;
				LOG("[info] Texture data stored into '%s' file.", _destPath);
			}
			RELEASE_ARRAY(data);
		}
		fclose(f);
	}
}

unsigned int ImporterMaterial::Save(CMaterial* _material, const char* _filename)
{
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _filename, "wb")) != 0) {
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s", _filename, strerror(err));
		return 0;
	}
	else {
		// header is the number of textures in the material
		unsigned int header = _material->textures.size();
		// header size + material parameters size   + size of texture struct * number of textures
		unsigned int size = sizeof(header) + sizeof(float) * 10 + sizeof(int) + (sizeof(int) * 6) * header;
		for (int i = 0; i < header; ++i)
		{
			size += sizeof(char) * (_material->textures[i]->type.size()+1 + _material->textures[i]->path.size()+1);
		}

		char* fileBuffer = new char[size];
		char* cursor = fileBuffer;

		unsigned int bytes = sizeof(header);
		memcpy(cursor, &header, bytes);
		cursor += bytes;

		// Store ambient, diffuse, specular settings
		bytes = sizeof(float) * 3;
		memcpy(cursor, _material->ambient.ptr(), bytes);
		cursor += bytes;

		memcpy(cursor, _material->diffuse.ptr(), bytes);
		cursor += bytes;

		memcpy(cursor, &_material->shininess, sizeof(float));
		cursor += sizeof(float);

		memcpy(cursor, _material->specular.ptr(), bytes);
		cursor += bytes;

		memcpy(cursor, &_material->shininessAlpha, sizeof(int));
		cursor += sizeof(int);

		for (int i = 0; i < header; ++i)
		{
			Texture* texture = _material->textures[i];
			int t[6] = {texture->type.size()+1, texture->path.size()+1, texture->wraps, texture->wrapt, texture->minfilter, texture->magfilter };
			bytes = sizeof(int) * 6;
			memcpy(cursor, t, bytes);
			cursor += bytes;

			bytes = sizeof(char) * t[0];
			memcpy(cursor, texture->type.c_str(), bytes);
			cursor += bytes;
			bytes = sizeof(char) * t[1];
			memcpy(cursor, texture->path.c_str(), bytes);
			cursor += bytes;
		}

		fwrite(fileBuffer, sizeof(char), size, f);
		LOG("[info] Material data stored into '%s' file", _filename);

		RELEASE_ARRAY(fileBuffer);
		fclose(f);
		return size;
	}
}

bool ImporterMaterial::Load(std::string _filename, CMaterial* _cmaterial, unsigned int _filesize)
{
	// Open file to read
	FILE* f;
	errno_t err;
	if ((err = fopen_s(&f, _filename.c_str(), "rb")) != 0) {
		// File could not be opened. FILE* was set to NULL. error code is returned in err.
		LOG("[error] File could not be opened: %s. %s", _filename.c_str(), strerror(err));
		return false;
	}
	else {
		if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
		{
			return false;
		}
		ilInit();
		iluInit();
		_cmaterial->filename = _filename.substr(0, _filename.find_last_of('/')).size() == _filename.size() ? _filename.substr(_filename.find_last_of('\\') + 1, _filename.size()) : _filename.substr(_filename.find_last_of('/') + 1, _filename.size());
		_cmaterial->filesize = _filesize;

		char* fileBuffer = new char[_filesize]; // Allocate
		char* cursor = fileBuffer;
		fread_s(fileBuffer, sizeof(char) * _filesize, sizeof(char), _filesize, f);

		// amount of textures
		unsigned int header[1];
		memcpy(header, cursor, sizeof(unsigned int));
		cursor += sizeof(int);

		// Load ambient, diffuse, specular settings
		unsigned int bytes = sizeof(float) * 3;
		float* color = new float[3];
		memcpy(color, cursor, bytes);
		_cmaterial->ambient = float3(color);
		RELEASE_ARRAY(color);
		cursor += bytes;

		color = new float[3];
		memcpy(color, cursor, bytes);
		_cmaterial->diffuse = float3(color);
		RELEASE_ARRAY(color);
		cursor += bytes;

		float s;
		memcpy(&s, cursor, sizeof(float));
		_cmaterial->shininess = s;
		cursor += sizeof(float);

		color = new float[3];
		memcpy(color, cursor, bytes);
		_cmaterial->specular = float3(color);
		RELEASE_ARRAY(color);
		cursor += bytes;

		int a;
		memcpy(&a, cursor, sizeof(int));
		_cmaterial->shininessAlpha = a;
		cursor += sizeof(int);

		for (int i = 0; i < header[0]; ++i)
		{
			int* t = new int[6];
			bytes = sizeof(int) * 6;
			memcpy(t, cursor, bytes);
			Texture* texture = new Texture();
			texture->wraps = t[2];
			texture->wrapt = t[3];
			texture->minfilter = t[4];
			texture->magfilter = t[5];
			cursor += bytes;

			char* tp = new char[t[0]];
			bytes = sizeof(char) * t[0];
			memcpy(tp, cursor, bytes);
			texture->type = tp;
			RELEASE_ARRAY(tp);
			cursor += bytes;

			char* path = new char[t[1]];
			bytes = sizeof(char) * t[1];
			memcpy(path, cursor, bytes);
			texture->path = path;
			RELEASE_ARRAY(path);
			cursor += bytes;
			RELEASE_ARRAY(t);

			texture->id = LoadTexture("./Library/Textures/" + texture->path, "./Library/Textures/" + texture->path);
			App->filesys->loadedTextures.push_back(texture);
			_cmaterial->textures.push_back(App->filesys->loadedTextures[App->filesys->loadedTextures.size() - 1]);
		}
		RELEASE_ARRAY(fileBuffer);
		LOG("[info] Material data loaded from '%s' file", _filename.c_str());
		fclose(f);

		ilShutDown();

		return true;
	}
	return false;
}

unsigned int ImporterMaterial::LoadTexture(const std::string _path, std::string _destPath, bool _saveToCustom)
{
	ILuint imgId;
	ILboolean success;
	ILinfo info;

	unsigned int textureId;

	ilGenImages(1, &imgId);								/* Generation of one image name */
	ilBindImage(imgId);									/* Binding of image name */
	success = ilLoadImage(_path.c_str());				/* Loading of image */
	if (success)										/* If no error occured: */
	{
		LOG("[info] Loaded texture from: %s", _path.c_str());
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success)
		{
			/* Error occured */
			LOG("[error] Could not convert image");
			return false;
		}
		iluGetImageInfo(&info);
		if (IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		glGenTextures(1, &textureId);				/* Texture name generation */
		glBindTexture(GL_TEXTURE_2D, textureId);	/* Binding of texture name */

		//ilGetInteger(IL_IMAGE_BPP)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
			ilGetData());							/* Texture specification */

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if(_saveToCustom)
			SaveTexture(_destPath.replace(_destPath.find_last_of('.'), _destPath.size(), ".dds").c_str());
	}
	else
	{
		/* Error occured */
		ILenum error;
		error = ilGetError();
		LOG("[error] Could not Load image %s. %s", _path.c_str(), iluErrorString(error));
			return false;
	}
	ilDeleteImages(1, &imgId);

	return textureId;
}

unsigned int ImporterMaterial::LoadCubemap(std::vector<std::string> _faces)
{
	ILuint imgId;
	ILboolean success;
	ILinfo info;
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < _faces.size(); i++)
	{
		ilGenImages(1, &imgId);								/* Generation of one image name */
		ilBindImage(imgId);									/* Binding of image name */
		success = ilLoadImage(_faces[i].c_str());			/* Loading of image */
		if (success)
		{
			success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			if (!success)
			{
				/* Error occured */
				LOG("[error] Could not convert image");
				return false;
			}
			iluGetImageInfo(&info);
			if (!IL_ORIGIN_UPPER_LEFT)
				iluFlipImage();

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8_ALPHA8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		}
		else
		{
			/* Error occured */
			LOG("[error] Could not Load image %s", _faces[i].c_str())
				return false;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
