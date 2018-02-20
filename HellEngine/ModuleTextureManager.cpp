#include "ModuleTextureManager.h"
#include "Application.h"
#include "ModuleRender.h"

ModuleTextureManager::ModuleTextureManager()
{
}

ModuleTextureManager::~ModuleTextureManager()
{
}

void ModuleTextureManager::GetTexture(const std::string &texturePath)
{
	for (std::map<std::string, TextureData>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if (texturePath.size() == it->first.size())
		{
			if (texturePath.compare(it->first))
			{
				it->second.numRefs++;
				return;
			}
		}
	}

	TextureData temp;
	temp.index = App->renderer->LoadImageWithDevIL(texturePath.c_str());
	temp.numRefs = 1;
	textures.insert(std::pair<std::string, TextureData>(texturePath, temp));
}

void ModuleTextureManager::ReleaseTexture(const std::string &texturePath)
{
	for (std::map<std::string, TextureData>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if (texturePath.size() == it->first.size())
		{
			if (texturePath.compare(it->first))
			{
				it->second.numRefs--;
				glDeleteTextures(1, &it->second.index);
				return;
			}
		}
	}
}