#include "Brofiler/include/Brofiler.h"
#include "SDL/include/SDL.h"
#include "Application.h"
#include "KeyState.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "UpdateStatus.h"
#include "globals.h"

ModuleRender::ModuleRender()
{
	camera.x = camera.y = 0;
	camera.w = SCREEN_WIDTH * SCREEN_SIZE;
	camera.h = SCREEN_HEIGHT* SCREEN_SIZE;
}

/* Destructor */
ModuleRender::~ModuleRender()
{}

/* Called before render is available */
bool ModuleRender::Init()
{
	LOGGER("Creating Renderer context");
	bool ret = true;
	Uint32 flags = 0;

	if(VSYNC == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->window->window, -1, flags);
	
	if(renderer == nullptr)
	{
		LOGGER("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

UpdateStatus ModuleRender::PreUpdate()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	return UpdateStatus::UPDATE_CONTINUE;
}

// Called every draw update
UpdateStatus ModuleRender::Update()
{
	// debug camera
	int speed = 1;

	if(App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT)
		App->renderer->camera.y += speed;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
		App->renderer->camera.y -= speed;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT)
		App->renderer->camera.x += speed;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT)
		App->renderer->camera.x -= speed;

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::PostUpdate()
{
	BROFILER_CATEGORY("ModuleRender - RenderPresent", Profiler::Color::Aqua)
	SDL_RenderPresent(renderer);
	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called before quitting */
bool ModuleRender::CleanUp()
{
	LOGGER("Destroying renderer");

	/* Destroy window */
	if(renderer != nullptr)
	{
		SDL_DestroyRenderer(renderer);
	}

	return true;
}

/* Blit to screen */
bool ModuleRender::Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed)
{
	bool ret = true;
	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * SCREEN_SIZE;
	rect.y = (int)(camera.y * speed) + y * SCREEN_SIZE;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= SCREEN_SIZE;
	rect.h *= SCREEN_SIZE;

	if(SDL_RenderCopy(renderer, texture, section, &rect) != 0)
	{
		LOGGER("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleRender::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool useCamera)
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (useCamera)
	{
		rec.x = (int)(camera.x + rect.x * SCREEN_SIZE);
		rec.y = (int)(camera.y + rect.y * SCREEN_SIZE);
		rec.w *= SCREEN_SIZE;
		rec.h *= SCREEN_SIZE;
	}

	if (SDL_RenderFillRect(renderer, &rec) != 0)
	{
		LOGGER("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}
