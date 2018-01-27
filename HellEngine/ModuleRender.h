#ifndef __H_MODULERENDER__
#define __H_MODULERENDER__

#include "SDL/include/SDL_rect.h"
#include "globals.h"
#include "Module.h"
struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;
typedef float GLfloat;
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
	void onWindowResize();

private:

	/* Initializes the GLEW library */
	bool InitGlew() const;

	/* Initializes OpenGL */
	bool InitOpenGL() const;

	/* Initializes cube-rendering variables */
	void InitCubeInfo();

	/* Initializes sphere-rendering variables */
	void InitSphereInfo(unsigned int rings, unsigned int sections);

	/* Creates a raw checkered texture and returns its OpenGL id */
	GLuint CreateCheckeredTexture();

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

	/* Draw a grid on the ground */
	void DrawGroundGrid() const;

	/* Initialize DevIL */
	void InitDevIL();

	/* Loading image with DevIL */
	GLuint LoadImageWithDevIL(const char* theFileName);

public:

	bool wireframe;
	bool shouldRotate;

private:
	
	static const int checkeredTextureSize = 64;

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

	GLfloat bottomLeft[2];
	GLfloat bottomRight[2];
	GLfloat topLeft[2];
	GLfloat topRight[2];

	GLuint vertexBufferId;
	GLuint colorsBufferId;
	GLuint uvCoordsBufferId;

	GLuint uniqueVerticesBufferId;
	GLuint uniqueColorsBufferId;
	GLuint uniqueUVCoordsBufferId;
	GLuint uniqueVerticesIndexBufferId;

	struct {
		unsigned int rings;
		unsigned int sections;
		unsigned int verticesCount;
		unsigned int trianglesCount;

		uint verticesBufferId;
		uint colorsBufferId;
		uint verticesIndexBufferId;
	} sphereInfo;

	GLuint checkeredTextureId;
	GLuint lennaTextureId;
	GLuint ryuTextureId;
	GLuint gokuTextureId;
	const char* lennaPath = "../Pictures/Lenna.png";
	const char* ryuPath = "../Pictures/ryu.jpg";
	const char* gokuPath = "../Pictures/goku.dds";
};

#endif /* __H_MODULERENDER__ */
