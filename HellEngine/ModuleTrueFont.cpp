#include "ModuleTrueFont.h"

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

bool ModuleTrueFont::CleanUp()
{
    TTF_Quit();

    return true;
}

TTF_Font* ModuleTrueFont::LoadFont(const char* path, int length)
{
    std::map<const char*, TTF_Font*>::iterator it = fonts.find(path);

    if (it != fonts.end()) return it->second;

    TTF_Font* font = TTF_OpenFont(path, length);

    if (font != nullptr) fonts[path] = font;

    return font;
}
