#ifndef __H_MODULERENDER__
#define __H_MODULERENDER__

#include "SDL/include/SDL_rect.h"
#include "Module.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:

	ModuleRender();
	~ModuleRender();

	bool Init();
	UpdateStatus PreUpdate();
	UpdateStatus Update();
	UpdateStatus PostUpdate();
	bool CleanUp();

	bool Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed = 1.0f);
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool useCamera = true);

public:

	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;
};

#endif /* __H_MODULERENDER__ */
