#include "SDL/include/SDL.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "globals.h"

ModuleWindow::ModuleWindow()
{
}

/* Destructor */
ModuleWindow::~ModuleWindow()
{
}

/* Called before render is available */
bool ModuleWindow::Init()
{
	LOGGER("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOGGER("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		/* Create window */
		int width = SCREEN_WIDTH;
		int height = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

		if(FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		SetupOpenGLAttributes();
		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == nullptr)
		{
			LOGGER("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

/* Called before quitting */
bool ModuleWindow::CleanUp()
{
	LOGGER("Destroying SDL window and quitting all SDL systems");

	/* Destroy window */
	if(window != nullptr)
	{
		SDL_DestroyWindow(window);
		surface = nullptr;
	}

	/* Quit SDL subsystems */
	SDL_Quit();
	return true;
}

/* Returns the window width */
int ModuleWindow::GetWidth() const
{
	return surface->w;
}

/* Returns the window height */
int ModuleWindow::GetHeight() const
{
	return surface->h;
}

void ModuleWindow::OnWindowResize()
{
	surface = SDL_GetWindowSurface(window);
}

/* Sets up the OpenGL attributes required before window creation */
void ModuleWindow::SetupOpenGLAttributes() const
{
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
}
