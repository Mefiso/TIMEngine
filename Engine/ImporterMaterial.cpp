#include "ImporterMaterial.h"
#include "GameObject.h"

#include <IL/il.h>
#include <IL/devil_cpp_wrapper.hpp>


bool ImporterMaterial::Import(const aiMaterial* _material, const std::string& _path, GameObject* _parent)
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

		RELEASE(diffuseMaps);
		RELEASE(specularMaps);
		ilShutDown();

		return true;
	}
	return false;
}

/*unsigned int ImporterMaterial::Save(const CMaterial* ourMaterial, const char* _filename)
{
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer
	if (size > 0)
	{
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			*fileBuffer = (char*)data;
	}
}*/

unsigned int ImporterMaterial::LoadTexture(const std::string path)
{
	ILuint imgId;
	ILboolean success;
	ILinfo info;

	unsigned int textureId;

	ilGenImages(1, &imgId);								/* Generation of one image name */
	ilBindImage(imgId);									/* Binding of image name */
	success = ilLoadImage(path.c_str());				/* Loading of image */
	if (success)										/* If no error occured: */
	{
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

		// here
	}
	else
	{
		/* Error occured */
		LOG("[error] Could not Load image %s", path.c_str())
			return false;
	}
	ilDeleteImages(1, &imgId);

	return textureId;
}

unsigned int ImporterMaterial::LoadCubemap(std::vector<std::string> faces)
{
	ILuint imgId;
	ILboolean success;
	ILinfo info;
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		ilGenImages(1, &imgId);								/* Generation of one image name */
		ilBindImage(imgId);									/* Binding of image name */
		success = ilLoadImage(faces[i].c_str());			/* Loading of image */
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
			LOG("[error] Could not Load image %s", faces[i].c_str())
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
