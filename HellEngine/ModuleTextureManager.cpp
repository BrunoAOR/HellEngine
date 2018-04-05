#pragma comment( lib, "Glew/libx86/glew32.lib" )
#pragma comment( lib, "DevIL/libx86/DevIL.lib" )
#pragma comment( lib, "DevIL/libx86/ILU.lib" )
#pragma comment( lib, "DevIL/libx86/ILUT.lib" )
#include "ModuleTextureManager.h"
#include "Application.h"
#include "ModuleRender.h"
#include "TextureInfo.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

ModuleTextureManager::ModuleTextureManager()
{
}

ModuleTextureManager::~ModuleTextureManager()
{
	for (std::map<std::string, TextureData>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		LOGGER("There were %d %s textures to be released, they will be removed in ModuleTextureManager destructor", it->second.numRefs, it->first.c_str());
		glDeleteTextures(1, &it->second.index);
	}
}

bool ModuleTextureManager::Init()
{
	InitDevIL();
	return true;
}

void ModuleTextureManager::InitDevIL()
{
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

/* Function load a image, turn it into a texture, and return the texture ID as a GLuint for use */
GLuint ModuleTextureManager::LoadImageWithDevIL(const char* theFileName, TextureInfo* outTextureInfo)
{
	ILuint imageID;				/* Create a image ID as a ULuint */
	GLuint textureID;			/* Create a texture ID as a GLuint */
	ILboolean success;			/* Create a flag to keep track of success/failure */
	ILenum error;				/* Create a flag to keep track of the IL error state */

	ilGenImages(1, &imageID); 		/* Generate the image ID */
	ilBindImage(imageID); 			/* Bind the image */
	success = ilLoadImage(theFileName); 	/* Load the image file */

											/* If we failed to open the image file in the first place... */
	if (!success)
	{
		ilBindImage(0);
		error = ilGetError();
		LOGGER("Image load failed - IL error: (%i) %s!", error, iluErrorString(error));
		return 0;
	}

	/* If we managed to load the image, then we can start to do things with it... */

	/* If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!) */
	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	/* ... then attempt to conver it. */
	/* NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
	success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	/* Quit out if we failed the conversion */
	if (!success)
	{
		error = ilGetError();
		LOGGER("Image conversion failed - IL error: (%i) %s!", error, iluErrorString(error));
		ilDeleteImages(1, &imageID);
		ilBindImage(0);
		return 0;
	}

	/* Save image width and height*/
	ILuint width, height, bytesPerPixel;
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	bytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	/* Generate a new texture */
	glGenTextures(1, &textureID);

	/* Bind the texture to a name */
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	/* Specify the texture specification */
	glTexImage2D(GL_TEXTURE_2D,			/* Type of texture */
		0,								/* Pyramid level (for mip-mapping) - 0 is the top level */
		ilGetInteger(IL_IMAGE_BPP),		/* Image colour depth */
		ilGetInteger(IL_IMAGE_WIDTH),	/* Image width */
		ilGetInteger(IL_IMAGE_HEIGHT),	/* Image height */
		0,								/* Border width in pixels (can either be 1 or 0) */
		ilGetInteger(IL_IMAGE_FORMAT),	/* Image format (i.e. RGB, RGBA, BGR etc.) */
		GL_UNSIGNED_BYTE,				/* Image data type */
		ilGetData()						/* The actual image data itself */
	);

	/* Unbind the texture to a name */
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	/* Because we have already copied image data into texture data we can release memory used by image. */
	ilDeleteImages(1, &imageID);
	ilBindImage(0);

	/* Save texture info */
	if (outTextureInfo)
	{
		outTextureInfo->width = width;
		outTextureInfo->height = height;
		outTextureInfo->bytesPerPixel = bytesPerPixel;
	}

	LOGGER("Texture creation successful.");

	return textureID;	/* Return the GLuint to the texture buffer */
}

GLuint ModuleTextureManager::GetTexture(const std::string &texturePath)
{
	for (std::map<std::string, TextureData>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if (texturePath.size() == it->first.size())
		{
			if (texturePath.compare(it->first) == 0)
			{
				it->second.numRefs++;
				return it->second.index;
			}
		}
	}

	GLuint textureId = LoadImageWithDevIL(texturePath.c_str());
	if (textureId != 0)
	{
		TextureData temp;
		temp.index = textureId;
		temp.numRefs = 1;
		textures.insert(std::pair<std::string, TextureData>(texturePath, temp));
	}

	return textureId;
}

void ModuleTextureManager::ReleaseTexture(const GLuint textureIndex)
{
	for (std::map<std::string, TextureData>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if (it->second.index == textureIndex)
		{
			it->second.numRefs--;

			if (it->second.numRefs == 0)
			{
				glDeleteTextures(1, &it->second.index);
				textures.erase(it);
			}

			return;
		}
	}
}