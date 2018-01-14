#ifndef __H_MODULEFADETOBLACK__
#define __H_MODULEFADETOBLACK__

#include "SDL/include/SDL_stdinc.h"
#include "Module.h"

class ModuleFadeToBlack : public Module
{
public:

	ModuleFadeToBlack(bool startEnabled = true);
	~ModuleFadeToBlack();

	bool Start();
	UpdateStatus Update();
	void FadeToBlack(Module* moduleOn, Module* moduleOff = nullptr, float time = 1.0f);
	bool IsFading() const;

private:

	Uint32 startTime = 0;
	Uint32 totalTime = 0;
	bool fadingIn = true;
	Module* moduleOn = nullptr;
	Module* moduleOff = nullptr;
};

#endif /* __H_MODULEFADETOBLACK__ */
