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

	/* Returns the timeScale affected time in milliseconds */
	Uint32 TimeMS() const;

	/* Returns the timeScale affected time in seconds */
	float Time() const;

	/* Returns the timeScale affected deltaTime in milliseconds */
	Uint32 DeltaTimeMS() const;

	/* Returns the timeScale affected deltaTime in seconds */
	float DeltaTime() const;
	
	/* Returns the timeScale unaffected time in milliseconds */
	Uint32 UnscaledTimeMS() const;

	/* Returns the timeScale unaffected time in seconds */
	float UnscaledTime() const;
	
	/* Returns the timeScale unaffected deltaTime in milliseconds */
	Uint32 UnscaledDeltaTimeMS() const;

	/* Returns the timeScale unaffected deltaTime in seconds */
	float UnscaledDeltaTime() const;

	/* Returns the timeScale value */
	float GetTimeScale() const;

	/* Sets the timeScale value ensuring it is >= 0 */
	void GetTimeScale(float newTimeScale);

private:

	Uint32 time = 0;
	Uint32 deltaTime = 0;
	float timeScale = 1.0f;
	Uint32 unscaledTime = 0;
	Uint32 unscaledDeltaTime = 0;
};

#endif /* !__H_MODULE_TIME__ */
