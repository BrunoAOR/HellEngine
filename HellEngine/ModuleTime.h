#ifndef __H_MODULE_TIME__
#define __H_MODULE_TIME__

#include "SDL/include/SDL_stdinc.h"
#include "Module.h"

class ModuleTime : public Module
{
public:

	ModuleTime();

	/* Destructor */
	~ModuleTime();

	/* Called each loop iteration */
	UpdateStatus PreUpdate();

	/* Returns the timeScale affected time */
	Uint32 GetTime() const;

	/* Returns the timeScale affected deltaTime */
	Uint32 GetDeltaTime() const;

	/* Returns the timeScale unaffected time */
	Uint32 GetUnscaledTime() const;

	/* Returns the timeScale unaffected deltaTime */
	Uint32 GetUnscaledDeltaTime() const;

	/* Returns the timeScale value */
	float GetTimeScale() const;

	/* Sets the timeScale value ensuring it is >= 0 */
	void GetTimeScale(float newTimeScale);


private:
	Uint32 time = 0;
	Uint32 deltaTime = 0;
	float timeScale = 0;
	Uint32 unscaledTime = 0;
	Uint32 unscaledDeltaTime = 0;
};

#endif /* !__H_MODULE_TIME__ */
