#pragma comment( lib, "Glew/libx86/glew32.lib" )
#pragma comment( lib, "DevIL/libx86/DevIL.lib" )
#pragma comment( lib, "DevIL/libx86/ILU.lib" )
#pragma comment( lib, "DevIL/libx86/ILUT.lib" )
#define SP_ARR_2F(x) x[0], x[1]
#define SP_ARR_3F(x) x[0], x[1], x[2]
#include <assert.h>
#include <math.h>
#include <vector>
#include "Brofiler/include/Brofiler.h"
#include "SDL/include/SDL.h"
#include "Application.h"
#include "Color.h"
#include "KeyState.h"
#include "Material.h"
#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "Shader.h"
#include "UpdateStatus.h"
#include "globals.h"
#include "openGL.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

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
			else
				InitDevIL();
		}
	}

	if (ret)
	{
		InitCubeInfo();
		InitSphereInfo(32, 32);
		ret &= InitCubeShaderInfo();
		if (ret)
			ret &= InitTextures();

		groundGridInfo.active = true;
	}

	return ret;
}

UpdateStatus ModuleRender::PreUpdate()
{
	Color c = App->editorCamera->background;
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->editorCamera->GetProjectionMatrix());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->editorCamera->GetViewMatrix());

	if (App->input->GetKey(SDL_SCANCODE_1) == KeyState::KEY_DOWN)
		currentSelectedCube = 0;
	if (App->input->GetKey(SDL_SCANCODE_2) == KeyState::KEY_DOWN)
		currentSelectedCube = 1;
	if (App->input->GetKey(SDL_SCANCODE_3) == KeyState::KEY_DOWN)
		currentSelectedCube = 2;
	if (App->input->GetKey(SDL_SCANCODE_4) == KeyState::KEY_DOWN)
		currentSelectedCube = 3;

	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called every draw update */
UpdateStatus ModuleRender::Update()
{
	if (shouldRotate)
	{
		rotationAngle += rotationSpeed * App->time->DeltaTime();
		if (rotationAngle > 360)
			rotationAngle -= 360;
	}

	float scale = 0.4f;

	/* Immediate Mode */
	{
		glPushMatrix();
		glTranslatef(-0.5f, 0.5f, 0);
		glScalef(scale, scale, scale);
		glRotatef(rotationAngle, -1, -1, 1);
		DrawCubeImmediateMode();
		glPopMatrix();
	}

	/* DrawArray */
	{
		glPushMatrix();
		glTranslatef(0.5f, 0.5f, 0);
		glScalef(scale, scale, scale);
		glRotatef(rotationAngle, -1, 1, -1);
		DrawCubeArrays();
		glPopMatrix();
	}

	/* DrawElements */
	{
		glPushMatrix();
		glTranslatef(-0.5f, -0.5f, 0);
		glScalef(scale, scale, scale);
		glRotatef(rotationAngle, 1, -1, -1);
		DrawCubeElements();
		glPopMatrix();
	}

	/* DrawRangeElements */
	{
		glPushMatrix();
		glTranslatef(0.5f, -0.5f, 0);
		glScalef(scale, scale, scale);
		glRotatef(rotationAngle, 1, 1, 1);
		DrawCubeRangeElements();
		glPopMatrix();
	}

	/* DrawSphere */
	{
		glPushMatrix();
		/* No translation required */
		glScalef(scale, scale, scale);
		glRotatef(rotationAngle, 1, 1, 1);
		DrawSphere();
		glPopMatrix();
	}

	/* DrawShaderCube */
	{
		glPushMatrix();
		DrawShaderCube();
		glPopMatrix();
	}

	/* DrawMaterialCube */
	{
		glPushMatrix();
		DrawMaterialCube();
		glPopMatrix();
	}

	/* DrawGroundGrid */
	{
		if (groundGridInfo.active)
		{
			DrawGroundGrid(App->editorCamera->getPosition()[0], App->editorCamera->getPosition()[2]);
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

	/* Destroy basicShader */
	if (basicShader != nullptr)
	{
		delete basicShader;
		basicShader = nullptr;
	}

	/* Destroy basicMaterial */
	if (basicMaterial != nullptr)
	{
		delete basicMaterial;
		basicMaterial = nullptr;
	}

	return true;
}

void ModuleRender::onWindowResize()
{
	glViewport(0, 0, App->window->getWidth(), App->window->getHeight());
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_FOG);

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

void ModuleRender::InitDevIL()
{
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

/* Initializes cube-rendering variables */
void ModuleRender::InitCubeInfo()
{
	/* rotationSpeed is expressed in degress per second */
	rotationSpeed = 90;
	/*
	CUBE drawing
	  G-----H
	 /|    /|
	C-----D |
	| |   | |
	| E---|-F
	|/    |/
	A-----B

	ABDC is the front face
	FEGH is the back face

	*/

	/* General */
	cubeSelectedTextures.push_back(0);
	cubeSelectedTextures.push_back(1);
	cubeSelectedTextures.push_back(2);
	cubeSelectedTextures.push_back(3);

	/* Cube vertices */
	{
		float s = 0.5f;

		vA[0] = -s;
		vA[1] = -s;
		vA[2] = s;

		vB[0] = s;
		vB[1] = -s;
		vB[2] = s;

		vC[0] = -s;
		vC[1] = s;
		vC[2] = s;

		vD[0] = s;
		vD[1] = s;
		vD[2] = s;

		vE[0] = -s;
		vE[1] = -s;
		vE[2] = -s;

		vF[0] = s;
		vF[1] = -s;
		vF[2] = -s;

		vG[0] = -s;
		vG[1] = s;
		vG[2] = -s;

		vH[0] = s;
		vH[1] = s;
		vH[2] = -s;
	}

	/* Cube colors */
	{
		cRed[0] = 1.0f;
		cRed[1] = 0;
		cRed[2] = 0;

		cGreen[0] = 0;
		cGreen[1] = 1.0f;
		cGreen[2] = 0;

		cBlue[0] = 0;
		cBlue[1] = 0;
		cBlue[2] = 1.0f;

		cWhite[0] = 1.0f;
		cWhite[1] = 1.0f;
		cWhite[2] = 1.0f;
	}

	/* UV coords */
	{
		bottomLeft[0] = 0.0f;
		bottomLeft[1] = 0.0f;

		bottomRight[0] = 1.0f;
		bottomRight[1] = 0.0f;

		topLeft[0] = 0.0f;
		topLeft[1] = 1.0f;

		topRight[0] = 1.0f;
		topRight[1] = 1.0f;
	}

	const size_t vertCount = 36;

	/* For DrawArray */
	GLfloat vertices[vertCount * 3] = { SP_ARR_3F(vA), SP_ARR_3F(vB), SP_ARR_3F(vC), SP_ARR_3F(vB), SP_ARR_3F(vD), SP_ARR_3F(vC), SP_ARR_3F(vB), SP_ARR_3F(vF), SP_ARR_3F(vD), SP_ARR_3F(vF), SP_ARR_3F(vH), SP_ARR_3F(vD), SP_ARR_3F(vF), SP_ARR_3F(vE), SP_ARR_3F(vH), SP_ARR_3F(vE), SP_ARR_3F(vG), SP_ARR_3F(vH), SP_ARR_3F(vE), SP_ARR_3F(vA), SP_ARR_3F(vG), SP_ARR_3F(vA), SP_ARR_3F(vC), SP_ARR_3F(vG), SP_ARR_3F(vC), SP_ARR_3F(vD), SP_ARR_3F(vH), SP_ARR_3F(vC), SP_ARR_3F(vH), SP_ARR_3F(vG), SP_ARR_3F(vA), SP_ARR_3F(vF), SP_ARR_3F(vB), SP_ARR_3F(vF), SP_ARR_3F(vA), SP_ARR_3F(vE) };
	GLfloat colors[vertCount * 3] = { SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cBlue) };
	GLfloat uvCoords[vertCount * 2] = { SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomRight), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topLeft) };

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, &colorsBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, colorsBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, &uvCoordsBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, uvCoordsBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 2, uvCoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	/* For DrawElements and DrawRangeElements */
	const size_t uniqueVertCount = 8 + 4;
	GLfloat uniqueVertices[uniqueVertCount * 3] = { SP_ARR_3F(vA), SP_ARR_3F(vB), SP_ARR_3F(vC), SP_ARR_3F(vD), SP_ARR_3F(vE), SP_ARR_3F(vF), SP_ARR_3F(vG), SP_ARR_3F(vH), SP_ARR_3F(vE), SP_ARR_3F(vF), SP_ARR_3F(vG), SP_ARR_3F(vH) };
	GLfloat uniqueColors[uniqueVertCount * 3] = { SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cRed) };
	GLfloat uniqueUVCoords[uniqueVertCount * 2] = { SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topRight), SP_ARR_2F(topLeft), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomRight) };
	GLubyte verticesOrder[vertCount] = { 0, 1, 2, 1, 3, 2,		/* Front face */
										1, 5, 3, 5, 7, 3,		/* Right face */
										5, 4, 7, 4, 6, 7,		/* Back face */
										4, 0, 6, 0, 2, 6,		/* Left face */
										2, 3, 11, 2, 11, 10,	/* Top face */
										0, 9, 1, 9, 0, 8 };		/* Botttom face */

	glGenBuffers(1, (GLuint*)&uniqueVerticesBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, uniqueVerticesBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 3, uniqueVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, (GLuint*)&uniqueColorsBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, uniqueColorsBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 3, uniqueColors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, (GLuint*)&uniqueUVCoordsBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, uniqueUVCoordsBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 2, uniqueUVCoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, (GLuint*)&uniqueVerticesIndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueVerticesIndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * vertCount, verticesOrder, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

bool ModuleRender::InitCubeShaderInfo()
{
	bool success = true;
	/* 
	For each vertex, we have:
	- 3 floats for position (x y z),
	- 3 floats for color (r g b),
	- 2 floats for texture coordinate (u v)
	*/


	const size_t vertCount = 36;
	GLfloat vertices[vertCount * 3] = { SP_ARR_3F(vA), SP_ARR_3F(vB), SP_ARR_3F(vC), SP_ARR_3F(vB), SP_ARR_3F(vD), SP_ARR_3F(vC), SP_ARR_3F(vB), SP_ARR_3F(vF), SP_ARR_3F(vD), SP_ARR_3F(vF), SP_ARR_3F(vH), SP_ARR_3F(vD), SP_ARR_3F(vF), SP_ARR_3F(vE), SP_ARR_3F(vH), SP_ARR_3F(vE), SP_ARR_3F(vG), SP_ARR_3F(vH), SP_ARR_3F(vE), SP_ARR_3F(vA), SP_ARR_3F(vG), SP_ARR_3F(vA), SP_ARR_3F(vC), SP_ARR_3F(vG), SP_ARR_3F(vC), SP_ARR_3F(vD), SP_ARR_3F(vH), SP_ARR_3F(vC), SP_ARR_3F(vH), SP_ARR_3F(vG), SP_ARR_3F(vA), SP_ARR_3F(vF), SP_ARR_3F(vB), SP_ARR_3F(vF), SP_ARR_3F(vA), SP_ARR_3F(vE) };
	GLfloat colors[vertCount * 3] = { SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cBlue) };
	GLfloat uvCoords[vertCount * 2] = { SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomRight), SP_ARR_2F(topLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomRight), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topLeft) };

	GLfloat allData[vertCount * 8];

	for (int i = 0; i < vertCount * 8; ++i)
	{
		if (i % 8 == 0 || i % 8 == 1 || i % 8 == 2 )
		{
			allData[i] = vertices[(i / 8) * 3 + (i % 8)];
		}
		else if (i % 8 == 3 || i % 8 == 4 || i % 8 == 5)
		{
			allData[i] = colors[(i / 8) * 3 + ((i % 8) - 3)];
		}
		else
		{
			allData[i] = uvCoords[(i / 8) * 2 + ((i % 8) - 6)];
		}
	}


	glGenBuffers(1, (GLuint*)&shaderDataBufferId);


	glBindBuffer(GL_ARRAY_BUFFER, shaderDataBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 8, allData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	// Shader time
	basicShader = new Shader();

	std::string vertexString;
	success &= LoadTextFile("assets/shaders/defaultShader.vert", vertexString);
	
	std::string fragmentString;
	success &= LoadTextFile("assets/shaders/defaultShader.frag", fragmentString);
	
	if (success)
	{
		success &= basicShader->CompileVertexShader(vertexString.c_str());
		success &= basicShader->CompileFragmentShader(fragmentString.c_str());
		if (success)
		{
			success &= basicShader->LinkShaderProgram();
		}
	}

	// Material
	basicMaterial = new Material();

	basicMaterial->SetVertexShaderPath("assets/shaders/defaultShader.vert");
	basicMaterial->SetFragmentShaderPath("assets/shaders/defaultShader.frag");
	basicMaterial->SetTexturePath("assets/images/ryu.jpg");

	success &= basicMaterial->Apply();

	return success;
}

void ModuleRender::InitSphereInfo(unsigned int rings, unsigned int sections)
{
	sphereInfo.rings = rings;
	sphereInfo.sections = sections;

	float radius = 1;
	sphereInfo.verticesCount = sections * (rings - 1) + 2;

	std::vector<GLfloat> vertices;
	vertices.resize(3 * sphereInfo.verticesCount);
	std::vector<GLfloat>::iterator v = vertices.begin();

	std::vector<GLfloat> colors;
	colors.resize(3 * sphereInfo.verticesCount);
	std::vector<GLfloat>::iterator c = colors.begin();

	float ringStep = (float)M_PI / rings;
	float sectionStep = 2 * (float)M_PI / sections;

	float redToGreenStep = 1.0f / rings;
	float blueStep = 2 * 1.0f / sections;
	/* VERTICES */

	/* First vertex is on the top*/
	*v++ = 0;
	*v++ = radius;
	*v++ = 0;

	*c++ = 1.0f;
	*c++ = 0;
	*c++ = 0;

	/* Now we iterate through rings, and within rings through sections */
	/* We start on ring 1, because ring 0 would be the top (special case) which has only 1 vertex and has already been handled*/
	for (unsigned int r = 1; r < rings; ++r)
	{
		float y = radius * cosf(ringStep * r);
		float ringSinf = radius * sinf(ringStep * r);

		float green = redToGreenStep * r;
		float red = 1.0f - green;

		for (unsigned int s = 0; s < sections; ++s)
		{
			float x = ringSinf * sinf(sectionStep * s);
			float z = ringSinf * cosf(sectionStep * s);
			*v++ = x;
			*v++ = y;
			*v++ = z;

			float blue = blueStep * s;
			if (blue > 1.0f)
				blue = 2.0f - blue;

			*c++ = red;
			*c++ = green;
			*c++ = blue;
		}
	}

	/* Last vertex is on the bottom*/
	*v++ = 0;
	*v++ = -radius;
	*v++ = 0;

	*c++ = 0;
	*c++ = 1.0f;
	*c++ = 0;

	assert(v == vertices.end() && c == colors.end());

	/* INDEXES */
	/*
	NOTE:
	The total number of triangles is 2 * sections * (rings - 1)
	This is due to the fact that the topmost and bottommost rings all join in the same top or bottom vertex.
	*/
	sphereInfo.trianglesCount = 2 * sections * (rings - 1);
	std::vector<GLuint> indexes;
	indexes.resize(3 * sphereInfo.trianglesCount);
	std::vector<GLuint>::iterator i = indexes.begin();

	/* Reminder: each group of 3 indexes define a triangle */
	unsigned int vRingMax = sphereInfo.verticesCount - 1;
	for (unsigned int vNum = 1; vNum < vRingMax + sections; ++vNum)
	{
		/* Triangle going up and then left from the vertex. Valid only if not on the first ring */
		if (vNum > sections) {
			/* Handle last ring */
			*i = vNum;
			if (*i > vRingMax)
				*i = vRingMax;
			++i;

			*i++ = vNum - sections;

			*i = vNum - sections - 1;
			/* Handle section wrap-around */
			if (*i % sections == 0)
				*i += sections;
			++i;
		}
		/* Triangle going right and then up-left from the vertex */
		if (vNum < vRingMax)
		{
			*i++ = vNum;

			*i = vNum + 1;
			/* Handle section wrap-around */
			if (vNum % sections == 0)
				*i -= sections;
			++i;

			int idx = (int)vNum - sections;
			/* Handle first ring */
			if (idx < 0)
				*i = 0;
			else
				*i = vNum - sections;
			++i;
		}

	}

	assert(i == indexes.end());

	/* Now we send the vertices and indexes to the VRAM */

	glGenBuffers(1, (GLuint*)&sphereInfo.verticesBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, sphereInfo.verticesBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphereInfo.verticesCount * 3, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, (GLuint*)&sphereInfo.colorsBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, sphereInfo.colorsBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphereInfo.verticesCount * 3, colors.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, (GLuint*)&sphereInfo.verticesIndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereInfo.verticesIndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * sphereInfo.trianglesCount * 3, indexes.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

bool ModuleRender::InitTextures()
{
	bool ret = true;
	GLuint checkeredTextureId;
	GLuint lennaTextureId;
	GLuint ryuTextureId;
	GLuint gokuTextureId;

	checkeredTextureId = CreateCheckeredTexture();

	uint idSum = 0;
	idSum += ryuTextureId = LoadImageWithDevIL(ryuPath);
	idSum += lennaTextureId = LoadImageWithDevIL(lennaPath);
	idSum += gokuTextureId = LoadImageWithDevIL(gokuPath);
	if (idSum == 0)
		ret = false;

	if (ret) {
		cubeTextureID.push_back(ryuTextureId);
		cubeTextureID.push_back(lennaTextureId);
		cubeTextureID.push_back(gokuTextureId);
		cubeTextureID.push_back(checkeredTextureId);
	}

	return ret;
}

GLuint ModuleRender::CreateCheckeredTexture()
{
	GLubyte checkImage[checkeredTextureSize][checkeredTextureSize][4];
	for (int i = 0; i < checkeredTextureSize; i++) {
		for (int j = 0; j < checkeredTextureSize; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	GLuint textureId;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkeredTextureSize, checkeredTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return textureId;
}

void ModuleRender::DrawCubeImmediateMode() const
{
	/*
	CUBE drawing
	  G-----H
	 /|    /|
	C-----D |
	| |   | |
	| E---|-F
	|/    |/
	A-----B

	ABDC is the front face
	FEGH is the back face

	*/
	GLuint tex = cubeTextureID.at(cubeSelectedTextures[0]);

	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_TRIANGLES);

	/* Front */
	glColor3fv(cRed);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vA);
	glColor3fv(cGreen);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vB);
	glColor3fv(cWhite);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vC);

	glColor3fv(cGreen);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vB);
	glColor3fv(cBlue);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vD);
	glColor3fv(cWhite);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vC);

	/* Right */
	glColor3fv(cGreen);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vB);
	glColor3fv(cWhite);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vF);
	glColor3fv(cBlue);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vD);

	glColor3fv(cWhite);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vF);
	glColor3fv(cRed);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vH);
	glColor3fv(cBlue);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vD);

	/* Back */
	glColor3fv(cWhite);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vF);
	glColor3fv(cBlue);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vE);
	glColor3fv(cRed);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vH);

	glColor3fv(cBlue);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vE);
	glColor3fv(cGreen);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vG);
	glColor3fv(cRed);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vH);

	/* Left */
	glColor3fv(cBlue);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vE);
	glColor3fv(cRed);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vA);
	glColor3fv(cGreen);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vG);

	glColor3fv(cRed);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vA);
	glColor3fv(cWhite);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vC);
	glColor3fv(cGreen);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vG);

	/* Top */
	glColor3fv(cWhite);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vC);
	glColor3fv(cBlue);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vD);
	glColor3fv(cRed);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vH);

	glColor3fv(cWhite);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vC);
	glColor3fv(cRed);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vH);
	glColor3fv(cGreen);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vG);

	/* Bottom */
	glColor3fv(cRed);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vA);
	glColor3fv(cWhite);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vF);
	glColor3fv(cGreen);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vB);

	glColor3fv(cWhite);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vF);
	glColor3fv(cRed);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vA);
	glColor3fv(cBlue);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vE);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void ModuleRender::DrawCubeArrays() const
{
	GLuint tex = cubeTextureID.at(cubeSelectedTextures[1]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, tex);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, colorsBufferId);
	glColorPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, uvCoordsBufferId);
	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRender::DrawCubeElements() const
{
	GLuint tex = cubeTextureID.at(cubeSelectedTextures[2]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, tex);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueVerticesBufferId);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueColorsBufferId);
	glColorPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueUVCoordsBufferId);
	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueVerticesIndexBufferId);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRender::DrawCubeRangeElements() const
{
	/*
	NOTE:
	Ideally, glDrawRangeElements would be called with a reduced amount of vertices' indexes (not 0 to 7 as here).
	But, since all the array of indices has already been pushed to the VRAM (and therefore the last parameter is GL_NONE),
	there is no real point in using glDrawRangeElements, since it is still going throught all the inidices.
	Regardless, it has been used here as a reminder of its existence.
	*/
	GLuint tex = cubeTextureID.at(cubeSelectedTextures[3]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, tex);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueVerticesBufferId);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueColorsBufferId);
	glColorPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueUVCoordsBufferId);
	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueVerticesIndexBufferId);
	glDrawRangeElements(GL_TRIANGLES, 0, 7, 36, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRender::DrawSphere() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, sphereInfo.verticesBufferId);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, sphereInfo.colorsBufferId);
	glColorPointer(3, GL_FLOAT, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereInfo.verticesIndexBufferId);
	glDrawElements(GL_TRIANGLES, 3 * sphereInfo.trianglesCount, GL_UNSIGNED_INT, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRender::DrawShaderCube() const
{
	basicShader->Activate();

	GLint modelLoc = glGetUniformLocation(basicShader->GetProgramId(), "model_matrix");

	float pos[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 2, 0, 1
	};

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, pos);

	GLint viewLoc = glGetUniformLocation(basicShader->GetProgramId(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->editorCamera->GetViewMatrix());

	GLint projLoc = glGetUniformLocation(basicShader->GetProgramId(), "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, App->editorCamera->GetProjectionMatrix());

	GLuint tex = cubeTextureID.at(cubeSelectedTextures[1]);

	glBindTexture(GL_TEXTURE_2D, tex);
	glBindBuffer(GL_ARRAY_BUFFER, shaderDataBufferId);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);

	basicShader->Deactivate();
}

void ModuleRender::DrawMaterialCube() const
{
	float pos[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, -2, 0, 1
	};

	basicMaterial->DrawArray(pos, shaderDataBufferId, 36);
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

/* Function load a image, turn it into a texture, and return the texture ID as a GLuint for use */
GLuint ModuleRender::LoadImageWithDevIL(const char* theFileName)
{
	ILuint imageID;				/* Create a image ID as a ULuint */

	GLuint textureID;			/* Create a texture ID as a GLuint */

	ILboolean success;			/* Create a flag to keep track of success/failure */

	ILenum error;				/* Create a flag to keep track of the IL error state */

	ilGenImages(1, &imageID); 		/* Generate the image ID */

	ilBindImage(imageID); 			/* Bind the image */

	success = ilLoadImage(theFileName); 	/* Load the image file */

	/* If we failed to open the image file in the first place... */
	if (!success)
	{
		ilBindImage(0);
		error = ilGetError();
		LOGGER("Image load failed - IL error: (%i) %s!", error, iluErrorString(error));
		return 0;
	}

	/* If we managed to load the image, then we can start to do things with it... */

	/* If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!) */
	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	/* ... then attempt to conver it. */
	/* NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
	success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	/* Quit out if we failed the conversion */
	if (!success)
	{
		error = ilGetError();
		LOGGER("Image conversion failed - IL error: (%i) %s!", error, iluErrorString(error));
		ilDeleteImages(1, &imageID);
		ilBindImage(0);
		return 0;
	}

	/* Save image width and height*/
	ILuint width, height, bytesPerPixel;
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	bytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	/* Generate a new texture */
	glGenTextures(1, &textureID);

	/* Bind the texture to a name */
	glBindTexture(GL_TEXTURE_2D, textureID);

	/* Set texture clamping method */
	switch (textureWrapMode) {
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	case 2:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		break;
	}

	/*Generate Mipmap from the current texture evaluated*/
	if (textureMipMapMode) {
		glGenerateMipmap(GL_TEXTURE_2D);
		glGenerateTextureMipmap(textureID);
	}

	/*Apply magnification filters if requested*/
	switch (textureMagnificationMode) {
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}
	/*Apply minification filters if requested*/
	switch (textureMinificationMode) {
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case 2:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case 5:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	}

	/* Specify the texture specification */
	glTexImage2D(GL_TEXTURE_2D,			/* Type of texture */
		0,		/* Pyramid level (for mip-mapping) - 0 is the top level */
		ilGetInteger(IL_IMAGE_BPP),		/* Image colour depth */
		ilGetInteger(IL_IMAGE_WIDTH),	/* Image width */
		ilGetInteger(IL_IMAGE_HEIGHT),	/* Image height */
		0,		/* Border width in pixels (can either be 1 or 0) */
		ilGetInteger(IL_IMAGE_FORMAT),	/* Image format (i.e. RGB, RGBA, BGR etc.) */
		GL_UNSIGNED_BYTE,		/* Image data type */
		ilGetData()				/* The actual image data itself */
	);

	/* Unbind the texture to a name */
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	/* Because we have already copied image data into texture data we can release memory used by image. */
	ilDeleteImages(1, &imageID);
	ilBindImage(0);

	/* Save texture info */
	textureInfo.insert(std::pair<int, Texture>(textureID, { width, height, bytesPerPixel }));

	LOGGER("Texture creation successful.");

	return textureID; /* Return the GLuint to the texture so you can use it! */
}

GLuint ModuleRender::GetTextureWidth()
{
	GLuint texID = cubeTextureID.at(cubeSelectedTextures[currentSelectedCube]);
	if (textureInfo.find(texID) != textureInfo.end()) {
		Texture tex = textureInfo.at(texID);
		return tex.width;
	}
	return 0;
}

GLuint ModuleRender::GetTextureHeight()
{
	GLuint texID = cubeTextureID.at(cubeSelectedTextures[currentSelectedCube]);
	if (textureInfo.find(texID) != textureInfo.end()) {
		Texture tex = textureInfo.at(texID);
		return tex.height;
	}
	return 0;
}

GLuint ModuleRender::GetBytesPerPixel()
{
	GLuint texID = cubeTextureID.at(cubeSelectedTextures[currentSelectedCube]);
	if (textureInfo.find(texID) != textureInfo.end()) {
		Texture tex = textureInfo.at(texID);
		return tex.bytesPerPixel;
	}
	return 0;
}

void ModuleRender::DeleteTexturesBuffer()
{
	for (std::vector<GLuint>::iterator it = cubeTextureID.begin(); it != cubeTextureID.end(); it++)
	{
		glDeleteTextures(1, &(*it));
	}

	cubeTextureID.clear();
	textureInfo.clear();
}

bool ModuleRender::ReloadTextures()
{

	bool ret = true;

	if (cubeTextureID.size() != 0) {
		DeleteTexturesBuffer();
	}

	GLuint checkeredTextureId;
	GLuint lennaTextureId;
	GLuint ryuTextureId;
	GLuint gokuTextureId;

	checkeredTextureId = CreateCheckeredTexture();

	uint idSum = 0;
	idSum += ryuTextureId = LoadImageWithDevIL(ryuPath);
	idSum += lennaTextureId = LoadImageWithDevIL(lennaPath);
	idSum += gokuTextureId = LoadImageWithDevIL(gokuPath);
	if (idSum == 0)
		ret = false;

	if (ret) {
		cubeTextureID.push_back(ryuTextureId);
		cubeTextureID.push_back(lennaTextureId);
		cubeTextureID.push_back(gokuTextureId);
		cubeTextureID.push_back(checkeredTextureId);
	}

	return ret;
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
