#include "SDL/include/SDL_timer.h"
#include "ModuleTime.h"
#include "UpdateStatus.h"

ModuleTime::ModuleTime() : Module(), timeScale(1.0f)
{}

/* Destructor */
ModuleTime::~ModuleTime()
{}

/* Called each loop iteration */
UpdateStatus ModuleTime::PreUpdate()
{
	Uint32 currentUnscaledTime = SDL_GetTicks();
	unscaledDeltaTime = currentUnscaledTime - unscaledTime;
	unscaledTime = currentUnscaledTime;

	deltaTime = (Uint32)(unscaledDeltaTime * timeScale);
	time += deltaTime;

	return UpdateStatus::UPDATE_CONTINUE;
}

/* Returns the timeScale affected time */
Uint32 ModuleTime::GetTime() const
{
	return time;
}

/* Returns the timeScale affected deltaTime */
Uint32 ModuleTime::GetDeltaTime() const
{
	return deltaTime;
}

/* Returns the timeScale unaffected time */
Uint32 ModuleTime::GetUnscaledTime() const
{
	return unscaledTime;
}

/* Returns the timeScale unaffected deltaTime */
Uint32 ModuleTime::GetUnscaledDeltaTime() const
{
	return unscaledDeltaTime;
}

/* Returns the timeScale value */
float ModuleTime::GetTimeScale() const
{
	return timeScale;
}

/* Sets the timeScale value ensuring it is >= 0 */
void ModuleTime::GetTimeScale(float newTimeScale)
{
	timeScale = newTimeScale >= 0 ? newTimeScale : 0;
}
