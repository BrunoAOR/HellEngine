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
	void onWindowResize();

	/* Get selected cube's texture width */
	GLuint GetTextureWidth();

	/* Get selected cube's texture height */
	GLuint GetTextureHeight();

	/* Get selected cube's texture bytes per pixel */
	GLuint GetBytesPerPixel();

	/* Erase information stored in vram's buffer related to the loaded textures*/
	void DeleteTexturesBuffer();

	/* Reloads all textures with the new specified parameters */
	bool ReloadTextures();

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

	/* Loading image with DevIL. */
	GLuint LoadImageWithDevIL(const char* theFileName);

private:

	/* Initializes the GLEW library */
	bool InitGlew() const;

	/* Initializes OpenGL */
	bool InitOpenGL() const;

	/* Initialize DevIL */
	void InitDevIL();

	/* Initializes cube-rendering variables */
	void InitCubeInfo();

	/* Initializes shader and material cube information */
	bool InitCubeShaderInfo();

	/* Initializes sphere-rendering variables */
	void InitSphereInfo(unsigned int rings, unsigned int sections);

	/* Loads textures */
	bool InitTextures();

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

	void DrawShaderCube() const;

	void DrawMaterialCube() const;

	/* Draw a grid on the ground */
	void DrawGroundGrid(float xOffset = 0, float zOffet = 0, int halfSize = 20) const;

public:

	bool shouldRotate;
	
	int currentSelectedCube = 0;
	int textureWrapMode = 0;
	int textureMipMapMode = 0;
	int textureMagnificationMode = 0;
	int textureMinificationMode = 0;

	struct {
		bool active;
		bool tracking;
		bool continuousTracking;
	} groundGridInfo;

	std::vector<int> cubeSelectedTextures;
	std::vector<Material*> materials;

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

	struct Texture {
		GLuint width;
		GLuint height;
		GLuint bytesPerPixel;
	};

	std::map<int, Texture> textureInfo;
	std::vector<GLuint> cubeTextureID;

	const char* lennaPath = "assets/images/lenna.png";
	const char* ryuPath = "assets/images/ryu.jpg";
	const char* gokuPath = "assets/images/goku.dds";

	GLuint shaderDataBufferId;
	Shader* basicShader = nullptr;
};

#endif /* __H_MODULERENDER__ */
