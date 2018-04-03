#include "SDL/include/SDL_timer.h"
#include "ModuleTime.h"
#include "UpdateStatus.h"

ModuleTime::ModuleTime() : Module()
{}

/* Destructor */
ModuleTime::~ModuleTime()
{}

/* Called each loop iteration */
UpdateStatus ModuleTime::PreUpdate()
{
	Uint32 currentUnscaledTime = SDL_GetTicks() - referenceTime;
	unscaledDeltaTime = currentUnscaledTime - unscaledTime;
	unscaledTime += unscaledDeltaTime;

	deltaTime = (Uint32)(unscaledDeltaTime * timeScale);
	time += deltaTime;

	return UpdateStatus::UPDATE_CONTINUE;
}

/* Returns the timeScale affected time */
Uint32 ModuleTime::TimeMS() const
{
	return time;
}

float ModuleTime::Time() const
{
	return time / 1000.0f;
}

/* Returns the timeScale affected deltaTime */
Uint32 ModuleTime::DeltaTimeMS() const
{
	return deltaTime;
}

float ModuleTime::DeltaTime() const
{
	return deltaTime / 1000.0f;
}

/* Returns the timeScale unaffected time */
Uint32 ModuleTime::UnscaledTimeMS() const
{
	return unscaledTime;
}

float ModuleTime::UnscaledTime() const
{
	return unscaledTime / 1000.0f;
}

/* Returns the timeScale unaffected deltaTime */
Uint32 ModuleTime::UnscaledDeltaTimeMS() const
{
	return unscaledDeltaTime;
}

float ModuleTime::UnscaledDeltaTime() const
{
	return unscaledDeltaTime / 1000.0f;
}

/* Returns the timeScale value */
float ModuleTime::GetTimeScale() const
{
	return timeScale;
}

/* Sets the timeScale value ensuring it is >= 0 */
void ModuleTime::SetTimeScale(float newTimeScale)
{
	timeScale = newTimeScale >= 0 ? newTimeScale : 0;
}

void ModuleTime::ResetReferenceTime()
{
	referenceTime = SDL_GetTicks();
	time = unscaledTime = 0;
}
