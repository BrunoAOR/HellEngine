#ifndef __H_MODULERENDER__
#define __H_MODULERENDER__

#include "SDL/include/SDL_rect.h"
#include "Module.h"
struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;
typedef void* SDL_GLContext;

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

public:

	/* Initializes the GLEW library */
	bool InitGlew() const;

	/* Initializes OpenGL */
	bool InitOpenGL() const;

	SDL_GLContext glContext = nullptr;
};

#endif /* __H_MODULERENDER__ */
