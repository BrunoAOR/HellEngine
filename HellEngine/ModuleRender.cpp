#include <assert.h>
#include <math.h>
#include <vector>
#include "Brofiler/include/Brofiler.h"
#include "SDL/include/SDL.h"
#include "Application.h"
#include "Color.h"
#include "ComponentCamera.h"
#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "UpdateStatus.h"
#include "globals.h"
#include "openGL.h"

ModuleRender::ModuleRender()
{}

/* Destructor */
ModuleRender::~ModuleRender()
{}

/* Called before render is available */
bool ModuleRender::Init()
{
	LOGGER("Creating OpenGL context");
	bool ret = true;

	glContext = SDL_GL_CreateContext(App->window->window);

	if (glContext == nullptr)
	{
		LOGGER("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		if (!InitGlew())
			ret = false;
		else
		{
			if (!InitOpenGL())
				ret = false;
		}
	}

	groundGridInfo.active = true;

	return ret;
}

UpdateStatus ModuleRender::PreUpdate()
{
	Color c = App->editorCamera->camera->GetBackground();
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->editorCamera->camera->GetProjectionMatrix());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->editorCamera->camera->GetViewMatrix());

	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called every draw update */
UpdateStatus ModuleRender::Update()
{
	/* DrawGroundGrid */
	{
		if (groundGridInfo.active)
		{
			DrawGroundGrid(App->editorCamera->camera->GetPosition()[0], App->editorCamera->camera->GetPosition()[2]);
		}
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::PostUpdate()
{
	BROFILER_CATEGORY("ModuleRender - GL_SwapWindow", Profiler::Color::Aqua);
	SDL_GL_SwapWindow(App->window->window);
	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called before quitting */
bool ModuleRender::CleanUp()
{
	LOGGER("Destroying OpenGL context");

	/* Destroy window */
	if (glContext != nullptr)
	{
		SDL_GL_DeleteContext(glContext);
		glContext = nullptr;
	}

	return true;
}

void ModuleRender::OnWindowResize()
{
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
}

bool ModuleRender::IsBackFaceCullActive()
{
	if (glIsEnabled(GL_CULL_FACE))
	{
		GLint faceMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &faceMode);
		return faceMode == GL_BACK || faceMode == GL_FRONT_AND_BACK;
	}
	return false;
}

/* Initializes the GLEW library */
bool ModuleRender::InitGlew() const
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		LOGGER("Error: glew could not be initialized. glewInit error: %s", glewGetErrorString(err));
		return false;
	}

	LOGGER("GLEW: Using Glew %s", glewGetString(GLEW_VERSION));

	LOGGER("Vendor: %s", glGetString(GL_VENDOR));
	LOGGER("Renderer: %s", glGetString(GL_RENDERER));
	LOGGER("OpenGL version supported %s", glGetString(GL_VERSION));
	LOGGER("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	return true;
}

/* Initializes OpenGL */
bool ModuleRender::InitOpenGL() const
{
	/* Reset projection and modelview matrices */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* Setup OpenGL options */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearDepth(1.0f);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_FOG);

	glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	/* Set viewport */
	//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	/* Check for errors */
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		LOGGER("Error initializing OpenGL! %s\n", glewGetErrorString(error));
		return false;
	}
	return true;
}

void ModuleRender::DrawGroundGrid(float xOffset, float zOffset, int halfSize) const
{
	if (!groundGridInfo.tracking)
	{
		xOffset = 0;
		zOffset = 0;
	}
	else if (!groundGridInfo.continuousTracking)
	{
		xOffset = round(xOffset);
		zOffset = round(zOffset);
	}

	int start = -halfSize;
	int extent = halfSize;
	float y = 0;

	glBegin(GL_LINES);

	for (int i = start; i <= extent; i++)
	{
		if (i == start) {
			glColor3f(.6f, .3f, .3f);
		}
		else {
			glColor3f(.25f, .25f, .25f);
		}

		glVertex3f((float)i + xOffset, y, (float)start + zOffset);
		glVertex3f((float)i + xOffset, y, (float)extent + zOffset);

		if (i == start) {
			glColor3f(.3f, .3f, .6f);
		}
		else {
			glColor3f(.25f, .25f, .25f);
		}

		glVertex3f((float)start + xOffset, y, (float)i + zOffset);
		glVertex3f((float)extent + xOffset, y, (float)i + zOffset);
	}

	glEnd();
}

void ModuleRender::ToggleOpenGLCapability(bool state, GLenum cap)
{
	if (state)
	{
		glEnable(cap);
	}
	else
	{
		glDisable(cap);
	}
}

void ModuleRender::SetFogMode(GLenum fogMode)
{
	glFogi(GL_FOG_MODE, fogMode);
}

void ModuleRender::SetFogDensity(float density)
{
	glFogf(GL_FOG_DENSITY, density);
}

void ModuleRender::setFogStartAndEnd(float start, float end)
{
	glFogf(GL_FOG_START, start);
	glFogf(GL_FOG_END, end);
}

void ModuleRender::SetFogColor(float * color)
{
	glFogfv(GL_FOG_COLOR, color);
}

void ModuleRender::SetAmbientLightColor(float * color)
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color);
}

void ModuleRender::SetLightColor(GLenum lightSymbolicName, float * color)
{
	glLightfv(lightSymbolicName, GL_AMBIENT, color);
}

void ModuleRender::SetPolygonDrawMode(GLenum drawMode)
{
	glPolygonMode(GL_FRONT_AND_BACK, drawMode);
}
