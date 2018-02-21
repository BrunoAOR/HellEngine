#ifndef __H_MODULE_TEXTURE_MANAGER__
#define __H_MODULE_TEXTURE_MANAGER__

#include "Module.h"
#include <map>
#include <string>
#include "openGL.h"

struct TextureData
{
	GLuint index;
	GLuint numRefs;
};

class ModuleTextureManager : public Module
{
public:
	ModuleTextureManager();
	~ModuleTextureManager();

public:
	GLuint GetTexture(const std::string &texturePath);
	void ReleaseTexture(const std::string &texturePath);

public:
	std::map<std::string, TextureData> textures;
};
#endif // !__H_MODULE_TEXTURE_MANAGER__