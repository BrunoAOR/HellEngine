#define SP_ARR_2F(x) x[0], x[1]
#define SP_ARR_3F(x) x[0], x[1], x[2]
#define _USE_MATH_DEFINES
#include <math.h>
#include "ImGui/imgui.h"
#include "ComponentMesh.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "globals.h"
#include "openGL.h"


ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	type = ComponentType::MESH;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	CreateCubeVAO();
	CreateSphereVAO(32, 32);
	activeVao = 0;
	LOGGER("Component of type '%s'", GetString(type));
}

ComponentMesh::~ComponentMesh()
{
	LOGGER("Deleting Component of type '%s'", GetString(type));
}

ComponentMesh::VaoInfo ComponentMesh::GetActiveVao() const
{
	return vaoInfos[activeVao];
}

bool ComponentMesh::SetActiveVao(uint index)
{
	if (index >= vaoInfos.size())
		return false;

	activeVao = index;
	return true;
}

void ComponentMesh::OnEditor()
{
	static bool optionsCreated = false;
	static std::string options = "";
	if (!optionsCreated)
	{
		optionsCreated = true;
		for (const VaoInfo& vaoInfo : vaoInfos)
		{
			options += vaoInfo.name;
			options += '\0';
		}
		options += '\0';
	}
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		ImGui::Combo("Selected Mesh", &activeVao, options.c_str());
	}
}

void ComponentMesh::CreateCubeVAO()
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

	/* Cube vertices */
	{
		const float s = 0.5f;

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

	const uint allVertCount = 36;
	const uint uniqueVertCount = 8 + 4;
	GLfloat uniqueVertices[uniqueVertCount * 3] = { SP_ARR_3F(vA), SP_ARR_3F(vB), SP_ARR_3F(vC), SP_ARR_3F(vD), SP_ARR_3F(vE), SP_ARR_3F(vF), SP_ARR_3F(vG), SP_ARR_3F(vH), SP_ARR_3F(vE), SP_ARR_3F(vF), SP_ARR_3F(vG), SP_ARR_3F(vH) };
	GLfloat uniqueColors[uniqueVertCount * 3] = { SP_ARR_3F(cRed), SP_ARR_3F(cGreen), SP_ARR_3F(cWhite), SP_ARR_3F(cBlue), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cRed), SP_ARR_3F(cBlue), SP_ARR_3F(cWhite), SP_ARR_3F(cGreen), SP_ARR_3F(cRed) };
	GLfloat uniqueUVCoords[uniqueVertCount * 2] = { SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomRight), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(topRight), SP_ARR_2F(topLeft), SP_ARR_2F(topLeft), SP_ARR_2F(topRight), SP_ARR_2F(bottomLeft), SP_ARR_2F(bottomRight) };
	GLubyte verticesOrder[allVertCount] = { 0, 1, 2, 1, 3, 2,		/* Front face */
		1, 5, 3, 5, 7, 3,		/* Right face */
		5, 4, 7, 4, 6, 7,		/* Back face */
		4, 0, 6, 0, 2, 6,		/* Left face */
		2, 3, 11, 2, 11, 10,	/* Top face */
		0, 9, 1, 9, 0, 8 };		/* Botttom face */

	float allUniqueData[uniqueVertCount * 8];

	for (int i = 0; i < uniqueVertCount * 8; ++i)
	{
		if (i % 8 == 0 || i % 8 == 1 || i % 8 == 2)
		{
			allUniqueData[i] = uniqueVertices[(i / 8) * 3 + (i % 8)];
		}
		else if (i % 8 == 3 || i % 8 == 4 || i % 8 == 5)
		{
			allUniqueData[i] = uniqueColors[(i / 8) * 3 + ((i % 8) - 3)];
		}
		else
		{
			allUniqueData[i] = uniqueUVCoords[(i / 8) * 2 + ((i % 8) - 6)];
		}
	}

	VaoInfo cubeVaoInfo;
	cubeVaoInfo.name = "Cube";
	cubeVaoInfo.elementsCount = allVertCount;
	cubeVaoInfo.indexesType = GL_UNSIGNED_BYTE;
	
	uint VBO;
	uint EBO;

	glGenVertexArrays(1, &cubeVaoInfo.vao);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(cubeVaoInfo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 8, allUniqueData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound, since the vertex information is stored in the VAO throught the VertexAttribPointers */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * cubeVaoInfo.elementsCount, verticesOrder, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Can be unbound AFTER unbinding the VAO, since the VAO stores information about the bound EBO */

	vaoInfos.push_back(cubeVaoInfo);
}

void ComponentMesh::CreateSphereVAO(uint rings, uint sections)
{
	VaoInfo sphereVaoInfo;
	sphereVaoInfo.name = "Sphere";

	float radius = 0.5f;
	uint verticesCount = sections * (rings - 1) + 2;

	std::vector<GLfloat> allData;
	allData.resize(8 * verticesCount);
	std::vector<GLfloat>::iterator d = allData.begin();

	float ringStep = (float)M_PI / rings;
	float sectionStep = 2 * (float)M_PI / sections;

	float redToGreenStep = 1.0f / rings;
	float blueStep = 2 * 1.0f / sections;
	
	/* VERTICES */

	/* First vertex is on the top*/
	*d++ = 0;
	*d++ = radius;
	*d++ = 0;

	*d++ = 1.0f;
	*d++ = 0;
	*d++ = 0;

	*d++ = 0;
	*d++ = 1.0f;

	/* Now we iterate through rings, and within rings through sections */
	/* We start on ring 1, because ring 0 would be the top (special case) which has only 1 vertex and has already been handled*/
	for (unsigned int r = 1; r < rings; ++r)
	{
		float y = radius * cosf(ringStep * r);
		float ringSinf = radius * sinf(ringStep * r);

		float green = redToGreenStep * r;
		float red = 1.0f - green;
		float v = 1.0f - (float)r / rings;

		for (unsigned int s = 0; s < sections; ++s)
		{
			float x = ringSinf * sinf(sectionStep * s);
			float z = ringSinf * cosf(sectionStep * s);
			*d++ = x;
			*d++ = y;
			*d++ = z;

			float blue = blueStep * s;
			if (blue > 1.0f)
				blue = 2.0f - blue;

			*d++ = red;
			*d++ = green;
			*d++ = blue;

			*d++ = (float)s / sections;
			*d++ = v;
		}
	}

	/* Last vertex is on the bottom*/
	*d++ = 0;
	*d++ = -radius;
	*d++ = 0;

	*d++ = 0;
	*d++ = 1.0f;
	*d++ = 0;

	*d++ = 0;
	*d++ = 0;

	assert(d == allData.end());

	/* INDEXES */
	/*
	NOTE:
	The total number of triangles is 2 * sections * (rings - 1)
	This is due to the fact that the topmost and bottommost rings all join in the same top or bottom vertex.
	*/
	uint trianglesCount = 2 * sections * (rings - 1);
	sphereVaoInfo.elementsCount = 3 * trianglesCount;
	sphereVaoInfo.indexesType = GL_UNSIGNED_INT;

	std::vector<GLuint> indexes;
	indexes.resize(sphereVaoInfo.elementsCount);
	std::vector<GLuint>::iterator i = indexes.begin();

	/* Reminder: each group of 3 indexes define a triangle */
	unsigned int vRingMax = verticesCount - 1;
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

	uint VBO;
	uint EBO;

	glGenVertexArrays(1, &sphereVaoInfo.vao);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(sphereVaoInfo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCount * 8, allData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound, since the vertex information is stored in the VAO throught the VertexAttribPointers */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * sphereVaoInfo.elementsCount, indexes.data(), GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Can be unbound AFTER unbinding the VAO, since the VAO stores information about the bound EBO */

	vaoInfos.push_back(sphereVaoInfo);
}
