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

bool SceneLoader::Load(const char* modelPath, GameObject* parent)
{
	assimpScene = aiImportFile(modelPath, aiProcess_Triangulate);
	if (parent == nullptr)
		parent = App->scene->root;

	if (assimpScene)
	{
		currentModelPath = modelPath;
		LoadMeshes();
		LoadNode(assimpScene->mRootNode, parent);
		StoreBoneToTransformLinks();

		aiReleaseImport(assimpScene);
		assimpScene = nullptr;
		currentModelPath = nullptr;
		moduleSceneMeshesOffset = 0;

		currentMeshInfos.clear();
		rootNode = nullptr;
		
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

	/* Cache the root node */
	if (!rootNode)
		rootNode = go;

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
		Each material will identify the meshInfo to draw by an index stored in ComponentMaterial:: */
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
		currentMeshInfos.push_back(meshInfo);
		App->scene->meshes.push_back(meshInfo);
	}
}

void SceneLoader::StoreBoneToTransformLinks()
{
	for (MeshInfo* meshInfo : currentMeshInfos)
	{
		for (Bone* bone : meshInfo->bones)
		{
			GameObject* go = rootNode->FindByName(bone->name);
			assert(go);
			ComponentTransform* transform = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
			assert(transform);

			bone->connectedTransform = transform;
		}
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
			
			bone->inverseBindMatrix = float4x4(
				abm.a1, abm.a2, abm.a3, abm.a4,
				abm.b1, abm.b2, abm.b3, abm.b4,
				abm.c1, abm.c2, abm.c3, abm.c4,
				abm.d1, abm.d2, abm.d3, abm.d4
			);
			
			bone->inverseBindMatrix.Transpose();

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
