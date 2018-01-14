#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )
#include "SDL_image/include/SDL_image.h"
#include "SDL/include/SDL.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "globals.h"

ModuleTextures::ModuleTextures()
{
}

/* Destructor */
ModuleTextures::~ModuleTextures()
{
	IMG_Quit();
}

/* Called before render is available */
bool ModuleTextures::Init()
{
	LOGGER("Init Image library");
	bool ret = true;

	/* Load support for the PNG image format */
	int flags = IMG_INIT_PNG;
	int init = IMG_Init(flags);

	if((init & flags) != flags)
	{
		LOGGER("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
		ret = false;
	}

	return ret;
}

/* Called before quitting */
bool ModuleTextures::CleanUp()
{
	LOGGER("Freeing textures and Image library");

	for(std::list<SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		SDL_DestroyTexture(*it);

	textures.clear();
	return true;
}

/* Load new texture from file path */
SDL_Texture* const ModuleTextures::Load(const char* path)
{
	SDL_Texture* texture = nullptr;
	SDL_Surface* surface = IMG_Load(path);

	if(surface == nullptr)
	{
		LOGGER("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}
	else
	{
		texture = SDL_CreateTextureFromSurface(App->renderer->renderer, surface);

		if(texture == nullptr)
		{
			LOGGER("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			textures.push_back(texture);
		}

		SDL_FreeSurface(surface);
	}

	return texture;
}

/* Free texture from memory */
void ModuleTextures::Unload(SDL_Texture* texture)
{
	for(std::list<SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
	{
		if(*it == texture)
		{
			SDL_DestroyTexture(*it);
			textures.erase(it);
			break;
		}
	}
}
