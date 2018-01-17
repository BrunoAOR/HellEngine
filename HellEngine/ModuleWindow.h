#ifndef __H_MODULEWINDOW__
#define __H_MODULEWINDOW__

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	/* Destructor */
	virtual ~ModuleWindow();

	/* Called before quitting */
	bool Init();

	/* Called before quitting */
	bool CleanUp();

public:

	/* Sets up the OpenGL attributes required before window creation */
	void setupOpenGLAttributes() const;
	
	/* The window we'll be rendering to */
	SDL_Window* window = nullptr;
};

#endif /* __H_MODULEWINDOW__ */
