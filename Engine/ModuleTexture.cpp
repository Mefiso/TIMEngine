#include "ModuleTexture.h"
#include "GL/glew.h"
#include "IL/il.h"
#include "IL/devil_cpp_wrapper.hpp"
#include "Leaks.h"

bool ModuleTexture::mipmap = true;
bool ModuleTexture::force_flip = false;

bool ModuleTexture::Init()
{
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		return false;
	}
	ilInit();
	iluInit();

	return true;
}

unsigned int ModuleTexture::LoadTexture(const std::string path)
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
		if (force_flip)
			iluFlipImage();

		glGenTextures(1, &textureId);				/* Texture name generation */
		glBindTexture(GL_TEXTURE_2D, textureId);	/* Binding of texture name */

		//ilGetInteger(IL_IMAGE_BPP)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
			ilGetData());							/* Texture specification */

		if (mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

unsigned int ModuleTexture::LoadCubemap(std::vector<std::string> faces)
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