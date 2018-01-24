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

	/* Called before render is available */
	bool Init();

	/* Called before quitting */
	bool CleanUp();

	/* Returns the window width */
	int getWidth() const;

	/* Returns the window height */
	int getHeight() const;

	/* Method to be called when the window is resized */
	void onWindowResize();

public:

	/* The window we'll be rendering to */
	SDL_Window* window = nullptr;

	/* The window surface */
	SDL_Surface* surface = nullptr;

private:

	/* Sets up the OpenGL attributes required before window creation */
	void SetupOpenGLAttributes() const;
};

#endif /* __H_MODULEWINDOW__ */
