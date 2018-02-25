#ifndef __H_MODULE_TEXTURE_MANAGER__
#define __H_MODULE_TEXTURE_MANAGER__

#include "Module.h"
#include <map>
#include <string>
#include "openGL.h"

struct TextureInfo;

struct TextureData
{
	GLuint index;
	uint numRefs;
};

class ModuleTextureManager : public Module
{
public:
	ModuleTextureManager();
	~ModuleTextureManager();

	bool Init();
	GLuint GetTexture(const std::string &texturePath);
	void ReleaseTexture(const GLuint textureIndex);

private:
	/* Initialize DevIL */
	void InitDevIL();

	/* Loading image with DevIL. */
	GLuint LoadImageWithDevIL(const char* theFileName, TextureInfo* textureInfo = nullptr);

private:
	std::map<std::string, TextureData> textures;
};
#endif // !__H_MODULE_TEXTURE_MANAGER__