#include "Application.h"
#include "ModuleAudio.h"
#include "globals.h"
#include "Bass/include/bass.h"

#pragma comment (lib,"Bass/libx86/bass.lib")

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
	

	return ret;
}

/* Called before quitting */
bool ModuleAudio::CleanUp()
{
	LOGGER("Freeing sound FX, closing Mixer and Audio subsystem");

	return true;
}


