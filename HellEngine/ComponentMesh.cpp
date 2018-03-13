#define _USE_MATH_DEFINES
#include <math.h>
#include "Brofiler/include/Brofiler.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ComponentAnimation.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentType.h"
#include "GameObject.h"
#include "ModelInfo.h"
#include "ModuleScene.h"
#include "MeshInfo.h"
#include "globals.h"
#include "openGL.h"

uint ComponentMesh::meshesCount = 0;
std::vector<ModelInfo> ComponentMesh::defaultModelInfos;

ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	type = ComponentType::MESH;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	if (meshesCount == 0)
	{
		App->scene->meshes.reserve(App->scene->meshes.size() + 2);
		CreateCubeMeshInfo();
		CreateSphereMeshInfo(32, 32);
	}
	++meshesCount;
	UpdateBoundingBox();
	//LOGGER("Component of type '%s'", GetString(type));
}

ComponentMesh::~ComponentMesh()
{
	//LOGGER("Deleting Component of type '%s'", GetString(type));
	--meshesCount;
	if (meshesCount == 0)
	{
		defaultModelInfos.clear();
	}
}

void ComponentMesh::Update()
{
	const ModelInfo* modelInfo = GetActiveModelInfo();
	if (modelInfo)
	{
		for (uint meshInfosIndex : modelInfo->meshInfosIndexes)
		{
			MeshInfo* meshInfo = App->scene->meshes[meshInfosIndex];
			if (!meshInfo->bones.empty())
			{
				ApplyVertexSkinning(meshInfo);
			}
		}
	}
}

const ModelInfo* ComponentMesh::GetActiveModelInfo() const
{
	if (activeModelInfo >= 0 && activeModelInfo < (int)defaultModelInfos.size())
		return &defaultModelInfos[activeModelInfo];

	if (activeModelInfo == 2 && customModelInfo.meshInfosIndexes.size() > 0)
		return &customModelInfo;

	return nullptr;
}

bool ComponentMesh::SetActiveModelInfo(int index)
{
	/* Both -1 and the defaultModelInfos size are allowed to account for the "None" and the "Custom Model" option */
	if (index < -1 || index > (int)defaultModelInfos.size())
		return false;

	activeModelInfo = index;
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	assert(transform);
	UpdateBoundingBox();
	return true;
}

void ComponentMesh::SetCustomModel(const ModelInfo& modelInfo)
{
	customModelInfo = modelInfo;
	activeModelInfo = 2;
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	assert(transform);
	UpdateBoundingBox();
}

void ComponentMesh::OnEditor()
{
	static int selectedMeshOption = 0;
	static const char* options = "None\0Cube\0Sphere\0Custom Model\0\0";

	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		selectedMeshOption = activeModelInfo + 1;
		if (ImGui::Combo("Selected Mesh", &selectedMeshOption, options))
			SetActiveModelInfo(selectedMeshOption - 1);

		if (activeModelInfo == 2 && customModelInfo.meshInfosIndexes.size() == 0)
		{
			ImGui::Text("No custom model found in this mesh!");
		}
	}
}

int ComponentMesh::MaxCountInGameObject()
{
	return 1;
}

void ComponentMesh::CreateCubeMeshInfo()
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

	int numCubeUniqueVertexes = 8;

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
	GLfloat uniqueVertices[uniqueVertCount * 3] = { SP_ARR_3(vA), SP_ARR_3(vB), SP_ARR_3(vC), SP_ARR_3(vD), SP_ARR_3(vE), SP_ARR_3(vF), SP_ARR_3(vG), SP_ARR_3(vH), SP_ARR_3(vE), SP_ARR_3(vF), SP_ARR_3(vG), SP_ARR_3(vH) };
	GLfloat uniqueColors[uniqueVertCount * 3] = { SP_ARR_3(cRed), SP_ARR_3(cGreen), SP_ARR_3(cWhite), SP_ARR_3(cBlue), SP_ARR_3(cBlue), SP_ARR_3(cWhite), SP_ARR_3(cGreen), SP_ARR_3(cRed), SP_ARR_3(cBlue), SP_ARR_3(cWhite), SP_ARR_3(cGreen), SP_ARR_3(cRed) };
	GLfloat uniqueUVCoords[uniqueVertCount * 2] = { SP_ARR_2(bottomLeft), SP_ARR_2(bottomRight), SP_ARR_2(topLeft), SP_ARR_2(topRight), SP_ARR_2(bottomRight), SP_ARR_2(bottomLeft), SP_ARR_2(topRight), SP_ARR_2(topLeft), SP_ARR_2(topLeft), SP_ARR_2(topRight), SP_ARR_2(bottomLeft), SP_ARR_2(bottomRight) };
	GLuint verticesOrder[allVertCount] = { 0, 1, 2, 1, 3, 2,		/* Front face */
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

	MeshInfo* cubeMeshInfo = new MeshInfo();
	cubeMeshInfo->name = "Cube";
	cubeMeshInfo->elementsCount = allVertCount;
	cubeMeshInfo->vertices = std::vector<float3>{ float3(vA[0], vA[1], vA[2]), float3(vB[0], vB[1], vB[2]), float3(vC[0], vC[1], vC[2]), float3(vD[0], vD[1], vD[2]), float3(vE[0], vE[1], vE[2]), float3(vF[0], vF[1], vF[2]), float3(vG[0], vG[1], vG[2]), float3(vH[0], vH[1], vH[2]) };
	cubeMeshInfo->indices = { 0, 1, 2, 1, 3, 2,		/* Front face */
		1, 5, 3, 5, 7, 3,		/* Right face */
		5, 4, 7, 4, 6, 7,		/* Back face */
		4, 0, 6, 0, 2, 6,		/* Left face */
		2, 3, 7, 2, 7, 6,	/* Top face */
		0, 5, 1, 5, 0, 4 };		/* Botttom face */

	glGenVertexArrays(1, &cubeMeshInfo->vao);
	glGenBuffers(1, &cubeMeshInfo->vbo);
	glGenBuffers(1, &cubeMeshInfo->ebo);

	glBindVertexArray(cubeMeshInfo->vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubeMeshInfo->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 8, allUniqueData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound, since the vertex information is stored in the VAO throught the VertexAttribPointers */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeMeshInfo->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * cubeMeshInfo->elementsCount, verticesOrder, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Can be unbound AFTER unbinding the VAO, since the VAO stores information about the bound EBO */

	App->scene->meshes.push_back(cubeMeshInfo);
	ModelInfo modelInfo;
	modelInfo.meshInfosIndexes.push_back(App->scene->meshes.size() - 1);
	defaultModelInfos.push_back(modelInfo);
}

void ComponentMesh::CreateSphereMeshInfo(uint rings, uint sections)
{
	MeshInfo* sphereMeshInfo = new MeshInfo();;
	sphereMeshInfo->name = "Sphere";

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
	sphereMeshInfo->vertices.push_back(float3(0, radius, 0));

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
			float3 vertex(x, y, z);
			sphereMeshInfo->vertices.push_back(vertex);

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
	sphereMeshInfo->vertices.push_back(float3(0, -radius, 0));

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
	sphereMeshInfo->elementsCount = 3 * trianglesCount;

	std::vector<GLuint> indexes;
	indexes.resize(sphereMeshInfo->elementsCount);
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

			sphereMeshInfo->indices.push_back(*i);
			++i;

			*i = vNum - sections;
			sphereMeshInfo->indices.push_back(*i);
			++i;

			*i = vNum - sections - 1;
			/* Handle section wrap-around */
			if (*i % sections == 0)
				*i += sections;

			sphereMeshInfo->indices.push_back(*i);
			++i;
		}
		/* Triangle going right and then up-left from the vertex */
		if (vNum < vRingMax)
		{
			*i = vNum;
			sphereMeshInfo->indices.push_back(*i);
			++i;

			*i = vNum + 1;
			/* Handle section wrap-around */
			if (vNum % sections == 0)
				*i -= sections;

			sphereMeshInfo->indices.push_back(*i);
			++i;

			int idx = (int)vNum - sections;
			/* Handle first ring */
			if (idx < 0)
				*i = 0;
			else
				*i = vNum - sections;

			sphereMeshInfo->indices.push_back(*i);
			++i;
		}
	}

	assert(i == indexes.end());
	
	/* Now we send the vertices and indexes to the VRAM */

	glGenVertexArrays(1, &sphereMeshInfo->vao);
	glGenBuffers(1, &sphereMeshInfo->vbo);
	glGenBuffers(1, &sphereMeshInfo->ebo);

	glBindVertexArray(sphereMeshInfo->vao);
	glBindBuffer(GL_ARRAY_BUFFER, sphereMeshInfo->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCount * 8, allData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound, since the vertex information is stored in the VAO throught the VertexAttribPointers */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereMeshInfo->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * sphereMeshInfo->elementsCount, indexes.data(), GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Can be unbound AFTER unbinding the VAO, since the VAO stores information about the bound EBO */

	App->scene->meshes.push_back(sphereMeshInfo);
	ModelInfo modelInfo;
	modelInfo.meshInfosIndexes.push_back(App->scene->meshes.size() - 1);
	defaultModelInfos.push_back(modelInfo);
}

void ComponentMesh::UpdateBoundingBox()
{
	ComponentTransform* transform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	if (transform)
		transform->UpdateBoundingBox(this);
}

struct matrix {
	matrix(float4x4 mgl)
		: a1(mgl[0][0]), a2(mgl[0][1]), a3(mgl[0][2]), a4(mgl[0][3])
		, b1(mgl[1][0]), b2(mgl[1][1]), b3(mgl[1][2]), b4(mgl[1][3])
		, c1(mgl[2][0]), c2(mgl[2][1]), c3(mgl[2][2]), c4(mgl[2][3])
		, d1(mgl[3][0]), d2(mgl[3][1]), d3(mgl[3][2]), d4(mgl[3][3])
	{}

	float a1, a2, a3, a4;
	float b1, b2, b3, b4;
	float c1, c2, c3, c4;
	float d1, d2, d3, d4;
};

void ComponentMesh::ApplyVertexSkinning(const MeshInfo * meshInfo)
{
	/* Find the ComponentAnimation in the siblings to identify the Root Node of the animation */
	std::vector<GameObject*> siblings = gameObject->GetParent()->GetChildren();
	ComponentAnimation* animationComponent = nullptr;
	BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning Find animComponent", Profiler::Color::Crimson);
	for (GameObject* sibling : siblings)
	{
		animationComponent = (ComponentAnimation*)sibling->GetComponent(ComponentType::ANIMATION);
		if (animationComponent)
			break;
	}

	if (animationComponent)
	{
		BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning Get root inverse", Profiler::Color::Crimson);
		GameObject* root = animationComponent->gameObject;

		/* Get the rootToWorldInverse matrix because all bone inverse matrices are in model (Root) coordinate space */
		ComponentTransform* rootTransform = (ComponentTransform*)root->GetComponent(ComponentType::TRANSFORM);
		assert(rootTransform);
		float4x4 rootToWorld = rootTransform->GetModelMatrix4x4();
		float4x4 rootToWorldInverse = rootToWorld.Inverted().Transposed();

		/* Get the pointer to the data in VRAM (map buffer) */
		glBindVertexArray(meshInfo->vao);
		glBindBuffer(GL_ARRAY_BUFFER, meshInfo->vbo);
		float* vramData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		/* Reset all vertices in the VRAM to ZERO */
		BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning Reset VRAM to zero", Profiler::Color::Crimson);
		uint verticesCount = meshInfo->vertices.size();
		for (uint v = 0; v < verticesCount; ++v)
		{
			memset(vramData + (v * 8), 0, sizeof(float) * 3);
			//vramData[v * 8 + 0] = 0;// meshInfo->vertices[v].x;// 0;
			//vramData[v * 8 + 1] = 0;// meshInfo->vertices[v].y;// 0;
			//vramData[v * 8 + 2] = 0;// meshInfo->vertices[v].z;// 0;

			/* TODO: Reset normals here */

			/* Leave UVcoords (positoins 6 & 7) untouched */
		}

		/* Iterate through bones and add their effect to the adecuate vertices */
		BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning Apply bone effects", Profiler::Color::Crimson);
		bool useVerticesGroupOptimization = true;
		if (useVerticesGroupOptimization)
		{
			for (std::map<std::vector<uint>, VerticesGroup>::const_iterator it = meshInfo->verticesGroups.begin(); it != meshInfo->verticesGroups.cend(); ++it)
			{
				const std::vector<uint>& vertexIndices = it->first;
				const VerticesGroup& verticesGroup = it->second;
				uint vectorSize = verticesGroup.bones.size();
				assert(vectorSize == verticesGroup.weights.size());

				float4x4 matrixSum = float4x4::zero;

				//BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning Loop over VerticesGroups", Profiler::Color::Crimson);
				for (uint i = 0; i < vectorSize; ++i)
				{
					BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning VerticeGroup For", Profiler::Color::Crimson);
					Bone* bone = verticesGroup.bones[i];

					//BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning GetModelMatrix", Profiler::Color::Crimson);
					//BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning MultiplyMatrices", Profiler::Color::Crimson);
					//float4x4 nodeTransformation = bone->connectedTransform->GetModelMatrix4x4() * rootToWorldInverse;
					float4x4 matrixProduct = bone->connectedTransform->GetModelMatrix4x4();
					matrixProduct.LeftMultiply(rootToWorldInverse);
					matrixProduct.RightMultiply(bone->inverseBindMatrix);
					matrixProduct *= verticesGroup.weights[i];

					//BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning Multiply by weight", Profiler::Color::Crimson);
					//matrixProduct = matrixProduct * verticesGroup.weights[i];

					//BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning Add MatrixSum", Profiler::Color::Crimson);
					matrixSum += matrixProduct;
				}

				//matrix mat(matrixSum);

				for (uint vertexIndex : vertexIndices)
				{
					//BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning COUNTER", Profiler::Color::Crimson);
					const float3& originalVertex = meshInfo->vertices[vertexIndex];

					//vramData[vertexIndex * 8 + 0] += mat.a1 * originalVertex.x + mat.b1 * originalVertex.y + mat.c1 * originalVertex.z + mat.d1;
					//vramData[vertexIndex * 8 + 1] += mat.a2 * originalVertex.x + mat.b2 * originalVertex.y + mat.c2 * originalVertex.z + mat.d2;
					//vramData[vertexIndex * 8 + 2] += mat.a3 * originalVertex.x + mat.b3 * originalVertex.y + mat.c3 * originalVertex.z + mat.d3;

					vramData[vertexIndex * 8 + 0] += matrixSum.v[0][0] * originalVertex.x + matrixSum.v[1][0] * originalVertex.y + matrixSum.v[2][0] * originalVertex.z + matrixSum.v[3][0];
					vramData[vertexIndex * 8 + 1] += matrixSum.v[0][1] * originalVertex.x + matrixSum.v[1][1] * originalVertex.y + matrixSum.v[2][1] * originalVertex.z + matrixSum.v[3][1];
					vramData[vertexIndex * 8 + 2] += matrixSum.v[0][2] * originalVertex.x + matrixSum.v[1][2] * originalVertex.y + matrixSum.v[2][2] * originalVertex.z + matrixSum.v[3][2];
				}
				BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning End", Profiler::Color::Crimson);

			}
		}
		else
		{
			for (Bone* bone : meshInfo->bones)
			{
				///* Find the GameObject (former aiNode) with the same name */
				//GameObject* nodeGo = root->FindByName(bone->name);
				//assert(nodeGo);
				///* Get the nodeGo transformation matrix in the coordinate space of the animation ROOT object */
				//ComponentTransform* nodeTransform = (ComponentTransform*)nodeGo->GetComponent(ComponentType::TRANSFORM);
				//assert(nodeTransform);

				float4x4 matrixProduct = bone->connectedTransform->GetModelMatrix4x4();
				matrixProduct.LeftMultiply(rootToWorldInverse);
				matrixProduct.RightMultiply(bone->inverseBindMatrix);
				matrixProduct.Transpose();

				matrix mat(matrixProduct);

				/* Iterate through all Bone weights and apply their effects */
				//BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning Apply individual weights", Profiler::Color::Crimson);
				uint weightsCount = bone->numWeights;
				uint vertexIndex = 0;

				for (uint w = 0; w < weightsCount; ++w)
				{
					BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning For not optimized", Profiler::Color::Crimson);
					/* Calculate effect */
					const BoneWeight& boneWeight = bone->weights[w];
					vertexIndex = boneWeight.vertexIndex;
					const float3& originalVertex = meshInfo->vertices[vertexIndex];
					//float3 vertexEffect = boneWeight.weight * matrixProduct.TransformPos(originalVertex);

					/* Apply effect */
					//BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning COUNTER", Profiler::Color::Crimson);
					vramData[vertexIndex * 8 + 0] += boneWeight.weight * (mat.a1 * originalVertex.x + mat.a2 * originalVertex.y + mat.a3 * originalVertex.z + mat.a4);
					vramData[vertexIndex * 8 + 1] += boneWeight.weight * (mat.b1 * originalVertex.x + mat.b2 * originalVertex.y + mat.b3 * originalVertex.z + mat.b4);
					vramData[vertexIndex * 8 + 2] += boneWeight.weight * (mat.c1 * originalVertex.x + mat.c2 * originalVertex.y + mat.c3 * originalVertex.z + mat.c4);

					//vramData[vertexIndex * 8 + 0] += vertexEffect.x;
					//vramData[vertexIndex * 8 + 1] += vertexEffect.y;
					//vramData[vertexIndex * 8 + 2] += vertexEffect.z;
				}

				BROFILER_CATEGORY("ComponentMesh::ApplyVertexSkinning End For not optimized", Profiler::Color::Crimson);
			}
		}

		/* Unmap buffer */
		glUnmapBuffer(GL_ARRAY_BUFFER);
		vramData = nullptr;
	}
}
