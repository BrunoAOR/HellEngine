#ifndef __H_MODULETEXTURES__
#define __H_MODULETEXTURES__

#include <list>
#include "Module.h"

struct SDL_Texture;

class ModuleTextures : public Module
{
public:

	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	SDL_Texture* const Load(const char* path);
	void Unload(SDL_Texture* texture);

private:

	std::list<SDL_Texture*> textures;
};

#endif /* __H_MODULETEXTURES__ */
