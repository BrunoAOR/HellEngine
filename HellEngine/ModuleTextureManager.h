#ifndef __H_MODULE_TEXTURE_MANAGER__
#define __H_MODULE_TEXTURE_MANAGER__

#include <map>
#include <string>
#include "openGL.h"

struct TextureData
{
	GLuint index;
	GLuint numRefs;
};

class ModuleTextureManager
{
public:
	ModuleTextureManager();
	~ModuleTextureManager();

private:
	void GetTexture(const std::string &texturePath);
	void ReleaseTexture(const std::string &texturePath);

private:
	std::map<std::string, TextureData> textures;
};
#endif // !__H_MODULE_TEXTURE_MANAGER__