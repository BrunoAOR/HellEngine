#ifndef __H_MODULERENDER__
#define __H_MODULERENDER__

#include <map>
#include <vector>
#include "SDL/include/SDL_rect.h"
#include "globals.h"
#include "Module.h"

class Material;
class Shader;
struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;
typedef float GLfloat;
typedef unsigned int GLenum;
typedef unsigned int GLuint;
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

	/* Method to be called when the window is resized */
	void OnWindowResize();

	/* Returns whether or not backface culling is active */
	bool IsBackFaceCullActive();

	/* Enables or disables an OpenGL capability */
	void ToggleOpenGLCapability(bool state, GLenum cap);

	/* Sets the fog mode (fogMode must be GL_LINEAR, GL_EXP or GL_EXP2) */
	void SetFogMode(GLenum fogMode);

	/* Sets the fog density (value between 0 and 1 is expected) */
	void SetFogDensity(float density);
	
	/* Sets the start and end distances for fog calculations */
	void setFogStartAndEnd(float start, float end);
	
	/* Sets the fog color to the specified RGBA value */
	void SetFogColor(float* color);

	/* Sets the ambient light color */
	void SetAmbientLightColor(float* color);

	/* Sets a light's color */
	void SetLightColor(GLenum lightSymbolicName, float* color);

	/* Sets the polygon draw mode (drawMode must be GL_FILL, GL_LINE or GL_POINT */
	void SetPolygonDrawMode(GLenum drawMode);

private:

	/* Initializes the GLEW library */
	bool InitGlew() const;

	/* Initializes OpenGL */
	bool InitOpenGL() const;

	/* Draw a grid on the ground */
	void DrawGroundGrid(float xOffset = 0, float zOffet = 0, int halfSize = 20) const;

public:

	struct {
		bool active;
		bool tracking;
		bool continuousTracking;
	} groundGridInfo;

private:
	
	SDL_GLContext glContext = nullptr;

};

#endif /* __H_MODULERENDER__ */
