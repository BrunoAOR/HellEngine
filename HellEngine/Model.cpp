#pragma comment( lib, "Assimp/libx86/assimp-vc140-mt.lib" )
#include <assert.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Application.h"
#include "Model.h"
#include "ModuleEditorCamera.h"
#include "ModuleRender.h"
#include "Shader.h"
#include "VAOInfo.h"
#include "openGL.h"

unsigned int Model::modelsCount = 0;
Shader* Model::shader = nullptr;
int Model::modelMatrixLoc = 0;
int Model::viewLoc = 0;
int Model::projectionLoc = 0;

bool LoadVertexShader(const char* vertexShaderPath, Shader* shader);
bool LoadFragmentShader(const char* fragmentShaderPath, Shader* shader);

Model::Model()
{
	if (modelsCount == 0)
	{
		shader = new Shader();
		LoadVertexShader("assets/shaders/defaultModelShader.vert", shader);
		LoadFragmentShader("assets/shaders/defaultModelShader.frag", shader);
		shader->LinkShaderProgram();
		assert(shader->IsValid());
		modelMatrixLoc = glGetUniformLocation(shader->GetProgramId(), "model_matrix");
		viewLoc = glGetUniformLocation(shader->GetProgramId(), "view");
		projectionLoc = glGetUniformLocation(shader->GetProgramId(), "projection");
	}
	++modelsCount;
}

Model::~Model()
{
	Clear();
	--modelsCount;
	if (modelsCount == 0)
	{
		delete shader;
		shader = nullptr;
		modelMatrixLoc = 0;
		viewLoc = 0;
		projectionLoc = 0;
	}
}

void Model::Load(const char* modelPath)
{
	Clear();
	assimpScene = aiImportFile(modelPath, 0);

	CreateTextureBuffers(modelPath);
	CreateVaoInfo();
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
	if (modelInfo.vaoInfos.size() > 0)
	{
		for (VaoInfo& vaoInfo : modelInfo.vaoInfos)
		{
			glDeleteVertexArrays(1, &vaoInfo.vao);
			glDeleteBuffers(1, &vaoInfo.vbo);
			glDeleteBuffers(1, &vaoInfo.ebo);
		}
		modelInfo.vaoInfos.clear();
	}
}

void Model::Draw() const
{
	shader->Activate();
	const float identity[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, identity);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->editorCamera->camera->GetViewMatrix());
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, App->editorCamera->camera->GetProjectionMatrix());

	for (unsigned int i = 0; i < assimpScene->mRootNode->mNumChildren; ++i)
	{
		DrawMesh(assimpScene->mRootNode->mChildren[i]->mMeshes[0]);
	}
	shader->Deactivate();
}

void Model::DrawMesh(unsigned int meshIndex) const
{
	const VaoInfo& vaoInfo = modelInfo.vaoInfos[meshIndex];

	glBindTexture(GL_TEXTURE_2D, vaoInfo.textureID);
	glBindVertexArray(vaoInfo.vao);
	glDrawElements(GL_TRIANGLES, vaoInfo.indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(GL_NONE);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Model::CreateTextureBuffers(const char* modelPath)
{
	int rootPathSize = strrchr(modelPath, '/') - modelPath + 1;
	numTextureBufferIds = assimpScene->mNumMaterials;
	textureBufferIds = new unsigned int[numTextureBufferIds];

	/* Load Textures */
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

void Model::CreateVaoInfo()
{
	int numVaoInfos = assimpScene->mNumMeshes;
	modelInfo.vaoInfos.reserve(numVaoInfos);
	for (unsigned int m = 0; m < assimpScene->mNumMeshes; ++m)
	{
		const aiMesh* assimpMesh = assimpScene->mMeshes[m];
		unsigned int materialIndex = assimpMesh->mMaterialIndex;

		VaoInfo vaoInfo;
		vaoInfo.textureID = textureBufferIds[materialIndex];

		/* Create temporary data buffers */
		const unsigned int allDataSize = assimpMesh->mNumVertices * 8;
		float* allData = new float [allDataSize];
		const unsigned int indexesSize = assimpMesh->mNumFaces * 3;
		int* indexes = new int[indexesSize];

		/* Fill in temporary data buffers */
		/*
		The order of information in allData is repeating sets of 8 elements as follows:
		vertex.x, vertex.y, vertex.z, normal.x, normal.y, normal.z, uvCoord.x, uvCoord.y
		*/

		/* Gather allData */
		for (unsigned int vertexIdx = 0; vertexIdx < assimpMesh->mNumVertices; ++vertexIdx)
		{
			const aiVector3D& vertex = assimpMesh->mVertices[vertexIdx];
			const aiVector3D& normal = assimpMesh->mNormals[vertexIdx];
			const aiVector3D& uvCoord = assimpMesh->mTextureCoords[0][vertexIdx];
			
			/* Store vertex in vaoInfo (for raycasting calculations) */
			vaoInfo.vertices.push_back(float3(vertex.x, vertex.y, vertex.z));

			/* Push vertex, normal and uvCoord to temporary data buffer */
			allData[vertexIdx * 8 + 0] = vertex.x;
			allData[vertexIdx * 8 + 1] = vertex.y;
			allData[vertexIdx * 8 + 2] = vertex.z;
			allData[vertexIdx * 8 + 3] = normal.x;
			allData[vertexIdx * 8 + 4] = normal.y;
			allData[vertexIdx * 8 + 5] = normal.z;
			allData[vertexIdx * 8 + 6] = uvCoord.x;
			allData[vertexIdx * 8 + 7] = uvCoord.y;
		}

		/* Gather indexes */
		vaoInfo.elementsCount = 0;
		for (unsigned int faceIdx = 0; faceIdx < assimpMesh->mNumFaces; ++faceIdx)
		{
			const aiFace& face = assimpMesh->mFaces[faceIdx];

			for (unsigned int i = 0; i < face.mNumIndices; ++i)
			{
				++vaoInfo.elementsCount;
				unsigned int index = face.mIndices[i];
				/* Store indices in vaoInfo (for raycasting calculations) */
				vaoInfo.indices.push_back(index);
				
				/* Push index to temporary data buffer */
				indexes[faceIdx * face.mNumIndices + i] = index;
			}

		}

		/* Transfer data from temporary buffers to VRAM */
		glGenVertexArrays(1, &vaoInfo.vao);
		glGenBuffers(1, &vaoInfo.vbo);
		glGenBuffers(1, &vaoInfo.ebo);

		glBindVertexArray(vaoInfo.vao);
		glBindBuffer(GL_ARRAY_BUFFER, vaoInfo.vbo);
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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vaoInfo.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexesSize, indexes, GL_STATIC_DRAW);

		glBindVertexArray(GL_NONE);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Must be unbound AFTER unbinding the VAO */


		/* Store the generated vaoInfo into the vaoInfos vector */
		modelInfo.vaoInfos.push_back(vaoInfo);

		/* Delete temporary data buffers */
		delete[] allData;
		allData = nullptr;
		delete[] indexes;
		indexes = nullptr;
	}
}

bool LoadVertexShader(const char* vertexShaderPath, Shader* shader)
{
	std::string vertexString;
	if (!LoadTextFile(vertexShaderPath, vertexString))
		return false;

	strncpy_s(shader->vertexPath, sizeof(shader->vertexPath), vertexShaderPath, sizeof(vertexShaderPath));

	if (!shader->CompileVertexShader(vertexString.c_str()))
		return false;

	return true;
}

bool LoadFragmentShader(const char* fragmentShaderPath, Shader* shader)
{
	std::string fragmentString;
	if (!LoadTextFile(fragmentShaderPath, fragmentString))
		return false;

	strncpy_s(shader->fragmentPath, sizeof(shader->fragmentPath), fragmentShaderPath, sizeof(fragmentShaderPath));

	if (!shader->CompileFragmentShader(fragmentString.c_str()))
		return false;

	return true;
}
