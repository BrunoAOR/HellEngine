#ifndef __H_MODULERENDER__
#define __H_MODULERENDER__

#include "SDL/include/SDL_rect.h"
#include "Module.h"
struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;
typedef float GLfloat;
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

private:

	/* Initializes the GLEW library */
	bool InitGlew() const;

	/* Initializes OpenGL */
	bool InitOpenGL() const;

	/* Initializes cube-rendering variables */
	void InitCubeInfo();

	void DrawCubeImmediateMode() const;

	void DrawCubeArrays() const;

	void DrawCubeElements() const;

	void DrawCubeRangeElements() const;

private:
	
	SDL_GLContext glContext = nullptr;
	float rotationAngle;
	float rotationSpeed;

	GLfloat vA[3];
	GLfloat vB[3];
	GLfloat vC[3];
	GLfloat vD[3];
	GLfloat vE[3];
	GLfloat vF[3];
	GLfloat vG[3];
	GLfloat vH[3];

	GLfloat cRed[3];
	GLfloat cGreen[3];
	GLfloat cBlue[3];
	GLfloat cWhite[3];

	unsigned int vertexBufferId;
	unsigned int colorsBufferId;

	unsigned int uniqueVerticesBufferId;
	unsigned int uniqueColorsBufferId;
	unsigned int uniqueVerticesIndexBufferId;
};

#endif /* __H_MODULERENDER__ */
