#include <math.h>
#include "SDL/include/SDL.h"
#include "Application.h"
#include "ModuleFadeToBlack.h"
#include "ModuleRender.h"
#include "UpdateStatus.h"
#include "globals.h"

ModuleFadeToBlack::ModuleFadeToBlack(bool startEnabled) :  Module(startEnabled)
{}

ModuleFadeToBlack::~ModuleFadeToBlack()
{}

/* Load assets */
bool ModuleFadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->renderer->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

/* Update: draw background */
UpdateStatus ModuleFadeToBlack::Update()
{
	if(startTime > 0)
	{
		Uint32 now = SDL_GetTicks() - startTime;
		float normalized = (float) now / (float) totalTime;
		
		if(normalized > 1.0f)
			normalized = 1.0f;
		
		if(fadingIn == false)
			normalized = 1.0f - normalized;

		SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, (Uint8) (normalized * 255.0f));
		SDL_RenderFillRect(App->renderer->renderer, NULL);

		if(moduleOff == nullptr && moduleOn != nullptr)
		{
			moduleOn->Enable();
			moduleOff = nullptr;
		}

		if(now >= totalTime)
		{
			if(fadingIn == true)
			{	
				if(moduleOff != nullptr)
					moduleOff->Disable();

				moduleOn->Enable();

				totalTime += totalTime;
				startTime = SDL_GetTicks();
				fadingIn = false;
			}
			else
			{
				startTime = 0;
			}
		}
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

/* Fade to black. At mid point deactivate one module, then activate the other */
void ModuleFadeToBlack::FadeToBlack(Module* moduleOn, Module* moduleOff, float time)
{
	fadingIn = (moduleOff != nullptr) ? true : false;
	startTime = SDL_GetTicks();
	totalTime = (Uint32) (time  * 0.5f * 1000.0f);

	this->moduleOn = moduleOn;
	this->moduleOff = moduleOff;
}

bool ModuleFadeToBlack::IsFading() const
{
	return startTime > 0;
}
