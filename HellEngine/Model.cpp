#pragma comment( lib, "Assimp/libx86/assimp-vc140-mt.lib" )
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Application.h"
#include "Model.h"
#include "ModuleRender.h"
#include "openGL.h"

Model::Model()
{
}

Model::~Model()
{
	Clear();
}

void Model::Load(const char* modelPath)
{
	Clear();
	assimpScene = aiImportFile(modelPath, 0);

	int rootPathSize = strrchr(modelPath, '/') - modelPath + 1;
	numTextureBufferIds = assimpScene->mNumMaterials;
	textureBufferIds = new unsigned int[numTextureBufferIds];
	for (unsigned int m = 0; m < assimpScene->mNumMaterials; ++m)
	{
		aiString aiPath;
		assimpScene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);
		char* fullPath = new char[rootPathSize + aiPath.length + 1];
		memcpy_s(fullPath, rootPathSize + aiPath.length + 1, modelPath, rootPathSize);
		memcpy_s(fullPath + rootPathSize, aiPath.length + 1, aiPath.data, aiPath.length);
		fullPath[rootPathSize + aiPath.length] = '\0';
		
		textureBufferIds[m] = App->renderer->LoadImageWithDevIL(fullPath);
		delete[] fullPath;
	}
}

void Model::Clear()
{
	if (assimpScene)
	{
		aiReleaseImport(assimpScene);
		assimpScene = nullptr;
	}
	if (numTextureBufferIds)
	{
		glDeleteTextures(numTextureBufferIds, textureBufferIds);
		delete[] textureBufferIds;
		textureBufferIds = nullptr;
		numTextureBufferIds = 0;
	}
}

void Model::Draw() const
{
	for (unsigned int i = 0; i < assimpScene->mRootNode->mNumChildren; ++i)
	{
		DrawMesh(assimpScene->mRootNode->mChildren[i]->mMeshes[0]);
	}
}

void Model::DrawMesh(unsigned int meshIndex) const
{
	const aiMesh* assimpMesh = assimpScene->mMeshes[meshIndex];
	unsigned int materialIndex = assimpMesh->mMaterialIndex;
	
	glBindTexture(GL_TEXTURE_2D, textureBufferIds[materialIndex]);
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < assimpMesh->mNumFaces; ++i)
	{
		const aiFace& face = assimpMesh->mFaces[i];

		for (unsigned int v = 0; v < face.mNumIndices; ++v)
		{
			unsigned int index = face.mIndices[v];
			const aiVector3D& vertex = assimpMesh->mVertices[index];
			const aiVector3D& normal = assimpMesh->mNormals[index];
			const aiVector3D& uvCoord = assimpMesh->mTextureCoords[0][index];
			glNormal3f(normal.x, normal.y, normal.z);
			glTexCoord3f(uvCoord.x, uvCoord.y, 0);
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}

	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}
