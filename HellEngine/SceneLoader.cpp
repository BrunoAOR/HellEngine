#pragma comment( lib, "Assimp/libx86/assimp-vc140-mt.lib")
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "openGL.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ModelInfo.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "SceneLoader.h"
#include "MeshInfo.h"

/* Temporary */
#include "ComponentAnimation.h"

SceneLoader::SceneLoader()
{
}

void SceneLoader::LoadCubeMesh()
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
}

void SceneLoader::LoadSphereMesh(unsigned int rings, unsigned int sections)
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
}

bool SceneLoader::Load(const char* modelPath, GameObject* parent, bool meshesOnly)
{
	assimpScene = aiImportFile(modelPath, aiProcess_Triangulate);
	if (parent == nullptr)
		parent = App->scene->root;

	if (assimpScene)
	{
		currentModelPath = modelPath;
		LoadMeshes();
		if (!meshesOnly)
		{
			LoadNode(assimpScene->mRootNode, parent);
			StoreBoneToTransformLinks();
		}

		aiReleaseImport(assimpScene);
		assimpScene = nullptr;
		currentModelPath = nullptr;
		moduleSceneMeshesOffset = 0;

		currentComponentMeshes.clear();
		
		return true;
	}
	else
	{
		return false;
	}
}

SceneLoader::~SceneLoader()
{
}

void SceneLoader::LoadNode(const aiNode* node, GameObject* parent)
{
	GameObject* go = App->scene->CreateGameObject();
	go->name = node->mName.data;
	go->SetParent(parent);

	/* Add Transform */
	ComponentTransform* transform = (ComponentTransform*)go->AddComponent(ComponentType::TRANSFORM);

	const aiMatrix4x4& aiLocalMatrix = node->mTransformation;
	float4x4 localMatrix(aiLocalMatrix.a1, aiLocalMatrix.b1, aiLocalMatrix.c1, aiLocalMatrix.d1, aiLocalMatrix.a2, aiLocalMatrix.b2, aiLocalMatrix.c2, aiLocalMatrix.d2, aiLocalMatrix.a3, aiLocalMatrix.b3, aiLocalMatrix.c3, aiLocalMatrix.d3, aiLocalMatrix.a4, aiLocalMatrix.b4, aiLocalMatrix.c4, aiLocalMatrix.d4);
	float3 position;
	Quat rotation;
	float3 scale;
	DecomposeMatrix(localMatrix, position, rotation, scale);

	transform->SetPosition(position.x, position.y, position.z);
	transform->SetRotation(rotation);
	transform->SetScale(scale.x, scale.y, scale.z);

	/* Add Material and Mesh if the node has mesh */
	if (node->mNumMeshes > 0)
	{
		char* fullPath = new char[256];

		/* NOTE: A GameObject will have as many materials as the number of sub-meshes (MeshInfo) contained in the aiNode (-> stored in ModelInfo)
		Each material will identify the meshInfo to draw by an index stored in ComponentMaterial::modelInfoVaoIndex */
		ModelInfo modelInfo;
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			unsigned int sceneMeshIndex = node->mMeshes[i];
			modelInfo.meshInfosIndexes.push_back(sceneMeshIndex + moduleSceneMeshesOffset);

			ComponentMaterial* material = (ComponentMaterial*)go->AddComponent(ComponentType::MATERIAL);
			material->SetDefaultMaterialConfiguration();
			unsigned int materialIndex = assimpScene->mMeshes[sceneMeshIndex]->mMaterialIndex;
			GetTextureFullPath(materialIndex, fullPath);
			material->SetTexturePath(fullPath);
			material->modelInfoVaoIndex = i;
			material->Apply();
		}

		delete[] fullPath;


		ComponentMesh* mesh = (ComponentMesh*)go->AddComponent(ComponentType::MESH);
		mesh->SetCustomModel(modelInfo);
		currentComponentMeshes.push_back(mesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		LoadNode(node->mChildren[i], go);
	}
}

void SceneLoader::LoadMeshes()
{
	moduleSceneMeshesOffset = App->scene->meshes.size();
	App->scene->meshes.reserve(moduleSceneMeshesOffset + assimpScene->mNumMeshes);
	for (unsigned int i = 0; i < assimpScene->mNumMeshes; ++i)
	{
		const aiMesh* assimpMesh = assimpScene->mMeshes[i];
		MeshInfo* meshInfo = CreateMeshInfo(assimpMesh);
		App->scene->meshes.push_back(meshInfo);
	}
}

void SceneLoader::StoreBoneToTransformLinks()
{
	for (ComponentMesh* mesh : currentComponentMeshes)
	{
		mesh->StoreBoneToTransformLinks();
	}
}

MeshInfo* SceneLoader::CreateMeshInfo(const aiMesh* assimpMesh)
{
	assert(assimpMesh->HasPositions());

	MeshInfo* meshInfo = new MeshInfo();
	
	GatherVerticesInfo(assimpMesh, meshInfo);
	GatherBonesInfo(assimpMesh, meshInfo);

	return meshInfo;
}

void SceneLoader::GatherVerticesInfo(const aiMesh* assimpMesh, MeshInfo* meshInfo)
{
	/* Create temporary data buffers */
	const unsigned int allDataSize = assimpMesh->mNumVertices * 8;
	float* allData = new float[allDataSize];
	const unsigned int indexesSize = assimpMesh->mNumFaces * 3;
	int* indexes = new int[indexesSize];

	/* Fill in temporary data buffers */
	/*
	The order of information in allData is repeating sets of 8 elements as follows:
	vertex.x, vertex.y, vertex.z, normal.x, normal.y, normal.z, uvCoord.x, uvCoord.y
	*/

	/* Gather allData */
	bool hasNormals = assimpMesh->HasNormals();
	bool hasUvCoords = assimpMesh->HasTextureCoords(0);

	for (unsigned int vertexIdx = 0; vertexIdx < assimpMesh->mNumVertices; ++vertexIdx)
	{
		const aiVector3D& vertex = assimpMesh->mVertices[vertexIdx];
		allData[vertexIdx * 8 + 0] = vertex.x;
		allData[vertexIdx * 8 + 1] = vertex.y;
		allData[vertexIdx * 8 + 2] = vertex.z;

		if (hasNormals)
		{
			const aiVector3D& normal = assimpMesh->mNormals[vertexIdx];
			allData[vertexIdx * 8 + 3] = normal.x;
			allData[vertexIdx * 8 + 4] = normal.y;
			allData[vertexIdx * 8 + 5] = normal.z;
		}
		else
		{
			allData[vertexIdx * 8 + 3] = 0;
			allData[vertexIdx * 8 + 4] = 1;
			allData[vertexIdx * 8 + 5] = 0;
		}

		if (hasUvCoords)
		{
			const aiVector3D& uvCoord = assimpMesh->mTextureCoords[0][vertexIdx];
			allData[vertexIdx * 8 + 6] = uvCoord.x;
			allData[vertexIdx * 8 + 7] = uvCoord.y;
		}
		else
		{
			allData[vertexIdx * 8 + 6] = 0;
			allData[vertexIdx * 8 + 7] = 0;
		}

		/* Store vertex in meshInfo (for raycasting calculations) */
		meshInfo->vertices.push_back(float3(vertex.x, vertex.y, vertex.z));
	}

	/* Gather indexes */
	meshInfo->elementsCount = 0;
	for (unsigned int faceIdx = 0; faceIdx < assimpMesh->mNumFaces; ++faceIdx)
	{
		const aiFace& face = assimpMesh->mFaces[faceIdx];
		assert(face.mNumIndices == 3);

		for (unsigned int i = 0; i < 3; ++i)
		{
			++meshInfo->elementsCount;
			unsigned int index = face.mIndices[i];
			/* Store indices in meshInfo (for raycasting calculations) */
			meshInfo->indices.push_back(index);

			/* Push index to temporary data buffer */
			indexes[faceIdx * face.mNumIndices + i] = index;
		}

	}

	/* Transfer data from temporary buffers to VRAM */
	glGenVertexArrays(1, &meshInfo->vao);
	glGenBuffers(1, &meshInfo->vbo);
	glGenBuffers(1, &meshInfo->ebo);

	glBindVertexArray(meshInfo->vao);
	glBindBuffer(GL_ARRAY_BUFFER, meshInfo->vbo);
	
	if (assimpMesh->HasBones())
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * allDataSize, allData, GL_DYNAMIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * allDataSize, allData, GL_STATIC_DRAW);

	/* vertex */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	/* normal */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	/* uvCoord */
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound before unbinding the VAO, because the glVertexAttribPointer preserves the VBO to VAO conection */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshInfo->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexesSize, indexes, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Must be unbound AFTER unbinding the VAO */

	/* Delete temporary data buffers */
	delete[] allData;
	allData = nullptr;
	delete[] indexes;
	indexes = nullptr;
}

void SceneLoader::GatherBonesInfo(const aiMesh* assimpMesh, MeshInfo* meshInfo)
{
	if (assimpMesh->HasBones())
	{
		meshInfo->bones.reserve(assimpMesh->mNumBones);
		for (uint i = 0; i < assimpMesh->mNumBones; ++i)
		{
			std::map<float, std::vector<uint>> boneWeightsGroup;

			aiBone* assimpBone = assimpMesh->mBones[i];
			Bone* bone = new Bone();

			uint nameLength = assimpBone->mName.length;
			char* auxName = new char[nameLength + 1];
			memcpy_s(auxName, nameLength, assimpBone->mName.C_Str(), nameLength);
			auxName[nameLength] = '\0';
			bone->name = auxName;

			bone->numWeights = assimpBone->mNumWeights;
			bone->weights = new BoneWeight[bone->numWeights];

			for (uint w = 0; w < bone->numWeights; ++w)
			{
				bone->weights[w].vertexIndex = assimpBone->mWeights[w].mVertexId;
				bone->weights[w].weight = assimpBone->mWeights[w].mWeight;
				boneWeightsGroup[bone->weights[w].weight].push_back(bone->weights[w].vertexIndex);
			}

			aiMatrix4x4& abm = assimpBone->mOffsetMatrix;
			
			/* Save matrix transposed */
			bone->inverseBindMatrix = float4x4(
				abm.a1, abm.b1, abm.c1, abm.d1,
				abm.a2, abm.b2, abm.c2, abm.d2,
				abm.a3, abm.b3, abm.c3, abm.d3,
				abm.a4, abm.b4, abm.c4, abm.d4
			);
			
			meshInfo->bones.push_back(bone);
			
			for (std::map<float, std::vector<uint>>::iterator it = boneWeightsGroup.begin(); it != boneWeightsGroup.end(); ++it)
			{
				VerticesGroup& verticesGroup = meshInfo->verticesGroups[it->second];
				verticesGroup.bones.push_back(bone);
				verticesGroup.weights.push_back(it->first);
			}
		}
	}
}

void SceneLoader::GetTextureFullPath(unsigned int materialIndex, char* fullPath)
{
	int rootPathSize = strrchr(currentModelPath, '/') - currentModelPath + 1;

	aiString aiPath;
	assimpScene->mMaterials[materialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);
	assert(rootPathSize + aiPath.length + 1 <= 256);

	memcpy_s(fullPath, rootPathSize + aiPath.length + 1, currentModelPath, rootPathSize);
	memcpy_s(fullPath + rootPathSize, aiPath.length + 1, aiPath.data, aiPath.length);
	fullPath[rootPathSize + aiPath.length] = '\0';
}
