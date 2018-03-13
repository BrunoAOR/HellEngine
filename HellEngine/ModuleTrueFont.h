#ifndef __H_MODULE_TRUEFONT__
#define __H_MODULE_TRUEFONT__

#include <map>
#include <SDL_ttf.h>

#include "Module.h"

class ModuleTrueFont : public Module
{
public:
	ModuleTrueFont();
	virtual ~ModuleTrueFont();

	bool Init();
    bool CleanUp();

    TTF_Font* LoadFont(const char* path, int length);

private:
    std::map<const char*, TTF_Font*> fonts;
};

#endif __H_MODULE_TRUEFONT__
