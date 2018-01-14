#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )
#include "SDL/include/SDL.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "globals.h"

ModuleAudio::ModuleAudio( bool startEnabled) : Module( startEnabled)
{}

/* Destructor */
ModuleAudio::~ModuleAudio()
{}

/* Called before render is available */
bool ModuleAudio::Init()
{
	LOGGER("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOGGER("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	/* Load support for the OGG format */
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOGGER("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	/* Initialize SDL_mixer */
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOGGER("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = false;
	}

	return ret;
}

//*Called before quitting */
bool ModuleAudio::CleanUp()
{
	LOGGER("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != nullptr)
	{
		Mix_FreeMusic(music);
	}

	for(std::vector<Mix_Chunk*>::iterator it = fx.begin(); it != fx.end(); ++it)
		Mix_FreeChunk(*it);

	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

/* Play a music file */
bool ModuleAudio::PlayMusic(const char* path, float fadeTime)
{
	bool ret = true;

	if(music != nullptr)
	{
		if(fadeTime > 0.0f)
		{
			Mix_FadeOutMusic((int) (fadeTime * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		/* this call blocks until fade out is done */
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == nullptr)
	{
		LOGGER("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fadeTime > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fadeTime * 1000.0f)) < 0)
			{
				LOGGER("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOGGER("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOGGER("Successfully playing %s", path);
	return ret;
}

/* Load WAV */
unsigned int ModuleAudio::LoadFx(const char* path)
{
	unsigned int ret = 0;
	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == nullptr)
	{
		LOGGER("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size() - 1;
	}

	return ret;
}

/* Play WAV */
bool ModuleAudio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if(id < fx.size())
	{
		Mix_PlayChannel(-1, fx[id], repeat);
		ret = true;
	}

	return ret;
}
