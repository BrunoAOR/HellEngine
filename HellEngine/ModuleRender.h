#ifndef __H_MODULERENDER__
#define __H_MODULERENDER__

#include "SDL/include/SDL_rect.h"
#include "globals.h"
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

	/* Initializes sphere-rendering variables */
	void InitSphereInfo(unsigned int rings, unsigned int sections);

	/* Renders a cube using OpenGL immediate mode */
	void DrawCubeImmediateMode() const;

	/* Renders a cube using OpenGL glDrawArray function */
	void DrawCubeArrays() const;

	/* Renders a cube using OpenGL glDrawElements function */
	void DrawCubeElements() const;

	/* Renders a cube using OpenGL glDrawRangeElements function */
	void DrawCubeRangeElements() const;

	/* Renders a sphere using OpenGL glDrawElements function */
	void DrawSphere() const;

public:

	bool wireframe;
	bool shouldRotate;

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

	uint vertexBufferId;
	uint colorsBufferId;

	uint uniqueVerticesBufferId;
	uint uniqueColorsBufferId;
	uint uniqueVerticesIndexBufferId;

	struct {
		unsigned int rings;
		unsigned int sections;
		unsigned int verticesCount;
		unsigned int trianglesCount;

		uint verticesBufferId;
		uint colorsBufferId;
		uint verticesIndexBufferId;
	} sphereInfo;
};

#endif /* __H_MODULERENDER__ */
