#include "ModuleTrueFont.h"
#include <SDL_ttf.h>
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib")


ModuleTrueFont::ModuleTrueFont()
{
}


ModuleTrueFont::~ModuleTrueFont()
{
}

bool ModuleTrueFont::Init()
{
	TTF_Init();

	return true;
}
