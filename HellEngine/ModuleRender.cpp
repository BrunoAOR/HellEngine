#pragma comment( lib, "Glew/libx86/glew32.lib" )
#define SP_ARR_3F(x) x[0], x[1], x[2]
#include <assert.h>
#include <math.h>
#include <vector>
#include "Brofiler/include/Brofiler.h"
#include "SDL/include/SDL.h"
#include "Application.h"
#include "KeyState.h"
#include "ModuleEditorCamera.h"
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

	if (ret)
	{
		InitCubeInfo();
		InitSphereInfo(32, 32);
	}

	return ret;
}

UpdateStatus ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->editorCamera->GetProjectionMatrix());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->editorCamera->GetViewMatrix());
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

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
		DrawCubeElements();
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

	/* DrawGroundGrid */
	{
		DrawGroundGrid();
	}

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	//glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

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


	const size_t vertCount = 36;
	
	/* For DrawArray */
	GLfloat vertices[vertCount * 3] = { SP_ARR_3F(vA), SP_ARR_3F(vB), SP_ARR_3F(vC), SP_ARR_3F(vB), SP_ARR_3F(vD), SP_ARR_3F(vC), SP_ARR_3F(vB), SP_ARR_3F(vF), SP_ARR_3F(vD), SP_ARR_3F(vF), SP_ARR_3F(vH), SP_ARR_3F(vD), SP_ARR_3F(vF), SP_ARR_3F(vE), SP_ARR_3F(vH), SP_ARR_3F(vE), SP_ARR_3F(vG), SP_ARR_3F(vH), SP_ARR_3F(vE), SP_ARR_3F(vA), SP_ARR_3F(vG), SP_ARR_3F(vA), SP_ARR_3F(vC), SP_ARR_3F(vG), SP_ARR_3F(vC), SP_ARR_3F(vD), SP_ARR_3F(vH), SP_ARR_3F(vC), SP_ARR_3F(vH), SP_ARR_3F(vG), SP_ARR_3F(vA), SP_ARR_3F(vF), SP_ARR_3F(vB), SP_ARR_3F(vF), SP_ARR_3F(vA), SP_ARR_3F(vE) };
	GLfloat colors[vertCount * 3] = { SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cRed), SP_ARR_3F(cBlue) };

	glGenBuffers(1, (GLuint*)&vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, (GLuint*)&colorsBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, colorsBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertCount * 3, colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	/* For DrawElements and DrawRangeElements */
	const size_t uniqueVertCount = 8;
	GLfloat uniqueVertices[uniqueVertCount * 3] = { SP_ARR_3F(vA), SP_ARR_3F(vB), SP_ARR_3F(vC), SP_ARR_3F(vD), SP_ARR_3F(vE), SP_ARR_3F(vF), SP_ARR_3F(vG), SP_ARR_3F(vH) };
	GLfloat uniqueColors[uniqueVertCount * 3] = { SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cRed) };
	GLubyte verticesOrder[vertCount] = {0, 1, 2, 1, 3, 2,
										1, 5, 3, 5, 7, 3,
										5, 4, 7, 4, 6, 7,
										4, 0, 6, 0, 2, 6,
										2, 3, 7, 2, 7, 6,
										0, 5, 1, 5, 0, 4};

	glGenBuffers(1, (GLuint*)&uniqueVerticesBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, uniqueVerticesBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 3, uniqueVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, (GLuint*)&uniqueColorsBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, uniqueColorsBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 3, uniqueColors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, (GLuint*)&uniqueVerticesIndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueVerticesIndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * vertCount, verticesOrder, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
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

void ModuleRender::DrawCubeImmediateMode() const
{
	glBegin(GL_TRIANGLES);

	/* Front */
	glColor3fv(cRed);
	glVertex3fv(vA);
	glColor3fv(cGreen);
	glVertex3fv(vB);
	glColor3fv(cWhite);
	glVertex3fv(vC);

	glColor3fv(cGreen);
	glVertex3fv(vB);
	glColor3fv(cBlue);
	glVertex3fv(vD);
	glColor3fv(cWhite);
	glVertex3fv(vC);

	/* Right */
	glColor3fv(cGreen);
	glVertex3fv(vB);
	glColor3fv(cWhite);
	glVertex3fv(vF);
	glColor3fv(cBlue);
	glVertex3fv(vD);

	glColor3fv(cWhite);
	glVertex3fv(vF);
	glColor3fv(cRed);
	glVertex3fv(vH);
	glColor3fv(cBlue);
	glVertex3fv(vD);

	/* Back */
	glColor3fv(cWhite);
	glVertex3fv(vF);
	glColor3fv(cBlue);
	glVertex3fv(vE);
	glColor3fv(cRed);
	glVertex3fv(vH);

	glColor3fv(cBlue);
	glVertex3fv(vE);
	glColor3fv(cGreen);
	glVertex3fv(vG);
	glColor3fv(cRed);
	glVertex3fv(vH);

	/* Left */
	glColor3fv(cBlue);
	glVertex3fv(vE);
	glColor3fv(cRed);
	glVertex3fv(vA);
	glColor3fv(cGreen);
	glVertex3fv(vG);

	glColor3fv(cRed);
	glVertex3fv(vA);
	glColor3fv(cWhite);
	glVertex3fv(vC);
	glColor3fv(cGreen);
	glVertex3fv(vG);

	/* Top */
	glColor3fv(cWhite);
	glVertex3fv(vC);
	glColor3fv(cBlue);
	glVertex3fv(vD);
	glColor3fv(cRed);
	glVertex3fv(vH);

	glColor3fv(cWhite);
	glVertex3fv(vC);
	glColor3fv(cRed);
	glVertex3fv(vH);
	glColor3fv(cGreen);
	glVertex3fv(vG);

	/* Bottom */
	glColor3fv(cRed);
	glVertex3fv(vA);
	glColor3fv(cWhite);
	glVertex3fv(vF);
	glColor3fv(cGreen);
	glVertex3fv(vB);

	glColor3fv(cWhite);
	glVertex3fv(vF);
	glColor3fv(cRed);
	glVertex3fv(vA);
	glColor3fv(cBlue);
	glVertex3fv(vE);

	glEnd();
}

void ModuleRender::DrawCubeArrays() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, colorsBufferId);
	glColorPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glDrawArrays(GL_TRIANGLES, 0, 36 * 3);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRender::DrawCubeElements() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueVerticesBufferId);
	glVertexPointer(3, GL_FLOAT, 0, GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueColorsBufferId);
	glColorPointer(3, GL_FLOAT, 0, GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueVerticesIndexBufferId);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

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
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueVerticesBufferId);
	glVertexPointer(3, GL_FLOAT, 0, GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, uniqueColorsBufferId);
	glColorPointer(3, GL_FLOAT, 0, GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueVerticesIndexBufferId);
	glDrawRangeElements(GL_TRIANGLES, 0, 7, 36, GL_UNSIGNED_BYTE, GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRender::DrawSphere() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, sphereInfo.verticesBufferId);
	glVertexPointer(3, GL_FLOAT, 0, GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ARRAY_BUFFER, sphereInfo.colorsBufferId);
	glColorPointer(3, GL_FLOAT, 0, GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereInfo.verticesIndexBufferId);
	glDrawElements(GL_TRIANGLES, 3 * sphereInfo.trianglesCount, GL_UNSIGNED_INT, GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRender::DrawGroundGrid() const
{
	int start = -20;
	int extent = 20;
	int y = 1.5;

	glBegin(GL_LINES);

	for (int i = start; i <= extent; i++)
	{
		if (i == start) {
			glColor3f(.6, .3, .3);
		}
		else {
			glColor3f(.25, .25, .25);
		}

		glVertex3f(i, y, start);
		glVertex3f(i, y, extent);

		if (i == start) {
			glColor3f(.3, .3, .6);
		}
		else {
			glColor3f(.25, .25, .25);
		}

		glVertex3f(start, y, i);
		glVertex3f(extent, y, i);
	}

	glEnd();
}
