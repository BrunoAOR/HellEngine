#include <random>
#include "ImGui/imgui.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/Quat.h"
#include "Application.h"
#include "Billboard.h"
#include "ComponentGrass.h"
#include "ModuleEditorCamera.h"
#include "ModuleTextureManager.h"
#include "ModuleScene.h"
#include "ModuleShaderManager.h"
#include "SerializableObject.h"
#include "ShaderProgram.h"
#include "openGL.h"

ComponentGrass::ComponentGrass(GameObject* owner) : Component(owner)
{
	type = ComponentType::GRASS;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	CreateQuadVAO();
	previousCameraPos.x = FLT_MAX;
	previousCameraPos.y = FLT_MAX;
	previousCameraPos.z = FLT_MAX;
	shaderProgram = App->shaderManager->GetShaderProgram("assets/shaders/defaultShader.vert", "assets/shaders/defaultShader.frag");
	assert(shaderProgram);
}

ComponentGrass::~ComponentGrass()
{
	App->shaderManager->ReleaseShaderProgram(shaderProgram);
	shaderProgram = nullptr;	

	if (textureID != 0)
		App->textureManager->ReleaseTexture(textureID);

	textureID = 0;

	for (Billboard* billboard : billboards)
		delete billboard;

	billboards.clear();
}

void ComponentGrass::Update()
{
	if (isValid)
	{
		const float3& currentCameraPos = App->editorCamera->camera->GetPosition3();
		if (billboardsChanged || previousCameraPos.x != currentCameraPos.x || previousCameraPos.z != currentCameraPos.z)
		{
			previousCameraPos = currentCameraPos;

			const uint allVertCount = 6;
			const uint uniqueVertCount = 4;
			const uint billboardsSize = billboards.size();

			GLfloat* newVertQuadData = new GLfloat[uniqueVertCount * 3 * billboardsSize];

			for (uint i = 0; i < billboardsSize; ++i)
			{
				Billboard* billboard = billboards[i];

				ComponentCamera* activeCamera = App->scene->GetActiveGameCamera();
				if (!activeCamera)
					activeCamera = App->editorCamera->camera;

				const Quad& quad = billboard->ComputeQuad(activeCamera);

				/* Quad vertices, color, and UV */
				for (uint j = 0; j < uniqueVertCount; ++j)
				{
					newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 0] = quad.vertices[j].x;
					newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 1] = quad.vertices[j].y;
					newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 2] = quad.vertices[j].z;
				}
			}

			glBindVertexArray(quadMeshInfo.vao);
			glBindBuffer(GL_ARRAY_BUFFER, quadMeshInfo.vbo);
			float* oldQuadData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			int j = 0;
			for (uint i = 0; i < uniqueVertCount * 8 * billboardsSize; ++i)
			{
				if (i % 8 == 0 || i % 8 == 1 || i % 8 == 2)
					oldQuadData[i] = newVertQuadData[j++];

			}

			glUnmapBuffer(GL_ARRAY_BUFFER);

			glBindVertexArray(GL_NONE);
			glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

			delete[] newVertQuadData;
		}
		DrawElements();
	}
}

void ComponentGrass::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::Checkbox("Active", &isActive);

		OnEditorBillboardConfiguration();
	}
}

int ComponentGrass::MaxCountInGameObject()
{
	return 1;
}

void ComponentGrass::Save(SerializableObject& obj) const
{
	Component::Save(obj);

	obj.AddString("TexturePath", texturePath);
	obj.AddFloat3("Position", position);
	obj.AddFloat("RandomPositionRange", randomPositionRange);
	obj.AddFloat("Width", width);
	obj.AddFloat("Height", height);
	obj.AddFloat("RandomScaleRange", randomScaleRange);
	obj.AddInt("BillboardInstancesX", billboardInstancesX);
	obj.AddInt("BillboardInstancesZ", billboardInstancesZ);
	obj.AddFloat("OffsetX", offsetX);
	obj.AddFloat("OffsetZ", offsetZ);
}

void ComponentGrass::Load(const SerializableObject& obj)
{
	Component::Load(obj);

	std::string texturePathAsString = obj.GetString("TexturePath");
	memcpy_s(texturePath, 256, texturePathAsString.c_str(), texturePathAsString.length());
	position = obj.GetFloat3("Position");
	randomPositionRange = obj.GetFloat("RandomPositionRange");
	width = obj.GetFloat("Width");
	height = obj.GetFloat("Height");
	randomScaleRange = obj.GetFloat("RandomScaleRange");
	billboardInstancesX = obj.GetInt("BillboardInstancesX");
	billboardInstancesZ = obj.GetInt("BillboardInstancesZ");
	offsetX = obj.GetFloat("OffsetX");
	offsetZ = obj.GetFloat("OffsetZ");

	LoadTexture();
	CreateQuadVAO();
}

void ComponentGrass::CreateQuadVAO()
{
	billboardsChanged = true;
	for (Billboard* billboard : billboards)
		delete billboard;

	billboards.clear();

	billboards.reserve(billboardInstancesX * billboardInstancesZ);

	float mainXOffset = -(offsetX * ((billboardInstancesX - 1) / 2.0f));
	float mainZOffset = -(offsetZ * ((billboardInstancesZ - 1) / 2.0f));

	for (int i = 0; i < billboardInstancesX; ++i)
	{
		for (int j = 0; j < billboardInstancesZ; ++j)
		{
			float xPositionRnd = (float)rand() / RAND_MAX;
			xPositionRnd = -(randomPositionRange / 2) + xPositionRnd * randomPositionRange;
			float zPositionRnd = (float)rand() / RAND_MAX;
			zPositionRnd = -(randomPositionRange / 2) + zPositionRnd * randomPositionRange;
			float scaleRnd = (float)rand() / RAND_MAX;
			scaleRnd = -(randomScaleRange / 2) + scaleRnd * randomScaleRange;


			Billboard* billboard = new Billboard();
			billboard->SetPosition(float3(position.x + offsetX * i + mainXOffset + xPositionRnd, position.y, position.z + offsetZ * j + mainZOffset + zPositionRnd));
			billboard->SetSize(width * (1 + scaleRnd), height * (1 + scaleRnd));
			billboards.push_back(billboard);
		}
	}

	GLfloat cWhite[3];

	GLfloat corners[8];

	{
		cWhite[0] = 1.0f;
		cWhite[1] = 1.0f;
		cWhite[2] = 1.0f;
	}

	/* UV coords */
	{
		corners[0] = 0.0f;
		corners[1] = 0.0f;

		corners[2] = 1.0f;
		corners[3] = 0.0f;

		corners[4] = 1.0f;
		corners[5] = 1.0f;

		corners[6] = 0.0f;
		corners[7] = 1.0f;
	}

	const uint allVertCount = 6;
	const uint uniqueVertCount = 4;
	const uint billboardsSize = billboards.size();

	GLfloat* uniqueQuadData = new GLfloat[uniqueVertCount * 8 * billboardsSize];
	GLuint* verticesOrder = new GLuint[allVertCount * billboardsSize];

	for (uint i = 0; i < billboardsSize; ++i)
	{
		Billboard* billboard = billboards[i];

		ComponentCamera* activeCamera = App->scene->GetActiveGameCamera();
		if (!activeCamera)
			activeCamera = App->editorCamera->camera;

		const Quad& quad = billboard->ComputeQuad(activeCamera);

		/* Quad vertices, color, and UV */
		for (int j = 0; j < uniqueVertCount; ++j)
		{
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 0] = quad.vertices[j].x;
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 1] = quad.vertices[j].y;
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 2] = quad.vertices[j].z;

			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 3] = cWhite[0];
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 4] = cWhite[1];
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 5] = cWhite[2];

			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 6] = corners[2 * j];
			uniqueQuadData[i * uniqueVertCount * 8 + (j * 8) + 7] = corners[2 * j + 1];
		}

		verticesOrder[i * allVertCount + 0] = (i * uniqueVertCount) + 0;
		verticesOrder[i * allVertCount + 1] = (i * uniqueVertCount) + 1;
		verticesOrder[i * allVertCount + 2] = (i * uniqueVertCount) + 2;
		verticesOrder[i * allVertCount + 3] = (i * uniqueVertCount) + 0;
		verticesOrder[i * allVertCount + 4] = (i * uniqueVertCount) + 2;
		verticesOrder[i * allVertCount + 5] = (i * uniqueVertCount) + 3;
	}

	quadMeshInfo.name = "Quad";
	quadMeshInfo.elementsCount = allVertCount * billboardsSize;

	glGenVertexArrays(1, &quadMeshInfo.vao);
	glGenBuffers(1, &quadMeshInfo.vbo);
	glGenBuffers(1, &quadMeshInfo.ebo);

	glBindVertexArray(quadMeshInfo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, quadMeshInfo.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 8 * billboardsSize, uniqueQuadData, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound, since the vertex information is stored in the VAO throught the VertexAttribPointers */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadMeshInfo.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * quadMeshInfo.elementsCount, verticesOrder, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Can be unbound AFTER unbinding the VAO, since the VAO stores information about the bound EBO */

	delete[] uniqueQuadData;
	delete[] verticesOrder;
}

void ComponentGrass::UpdateBillboards()
{
	billboardsChanged = true;
	assert(billboardInstancesX * billboardInstancesZ == billboards.size());

	float mainXOffset = -(offsetX * ((billboardInstancesX - 1) / 2.0f));
	float mainZOffset = -(offsetZ * ((billboardInstancesZ - 1) / 2.0f));

	for (int i = 0; i < billboardInstancesX; ++i)
	{
		for (int j = 0; j < billboardInstancesZ; ++j)
		{
			float xPositionRnd = (float)rand() / RAND_MAX;
			xPositionRnd = -(randomPositionRange / 2) + xPositionRnd * randomPositionRange;
			float zPositionRnd = (float)rand() / RAND_MAX;
			zPositionRnd = -(randomPositionRange / 2) + zPositionRnd * randomPositionRange;
			float scaleRnd = (float)rand() / RAND_MAX;
			scaleRnd = -(randomScaleRange / 2) + scaleRnd * randomScaleRange;

			Billboard* billboard = billboards[i * billboardInstancesZ + j];
			billboard->SetPosition(float3(position.x + offsetX * i + mainXOffset + xPositionRnd, position.y, position.z + offsetZ * j + mainZOffset + zPositionRnd));
			billboard->SetSize(width * (1 + scaleRnd), height * (1 + scaleRnd));
		}
	}
}

bool ComponentGrass::DrawElements()
{
	if (isValid)
	{
		shaderProgram->Activate();

		float4x4 modelMatrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			position.x, position.y, position.z, 1
		);
		shaderProgram->UpdateMatrixUniforms(modelMatrix.ptr(), App->editorCamera->camera->GetViewMatrix(), App->editorCamera->camera->GetProjectionMatrix());

		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(quadMeshInfo.vao);
		glDrawElements(GL_TRIANGLES, quadMeshInfo.elementsCount, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(GL_NONE);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		shaderProgram->Deactivate();
		return true;
	}

	return false;
}

bool ComponentGrass::LoadTexture()
{
	/* Adquire new texture before releasing previous one to avoid re-creation if the same texture is requested */
	uint oldTextureId = textureID;

	if (!IsEmptyString(texturePath))
		textureID = App->textureManager->GetTexture(texturePath);

	App->textureManager->ReleaseTexture(oldTextureId);
	oldTextureId = 0;

	isValid = shaderProgram && textureID != 0;

	return textureID != 0;
}

void ComponentGrass::OnEditorBillboardConfiguration()
{
	static const std::string baseLabel = std::string("Billboard configuration##BillboardConfig");
	if (ImGui::TreeNode((baseLabel + std::to_string(editorInfo.id)).c_str()))
	{
		ImGui::InputText("Texture path", texturePath, 256);
		if (ImGui::Button("Load Texture"))
			LoadTexture();

		ImGui::NewLine();

		if (ImGui::DragFloat3("Position", position.ptr(), 0.03f))
			UpdateBillboards();

		if (ImGui::InputFloat("Random Pos Rng", &randomPositionRange))
			UpdateBillboards();
		
		ImGui::NewLine();

		if (ImGui::InputFloat("Width", &width))
			UpdateBillboards();

		if (ImGui::InputFloat("Height", &height))
			UpdateBillboards();

		if (ImGui::InputFloat("Random Scale Rng", &randomScaleRange))
			UpdateBillboards();

		ImGui::NewLine();

		if (ImGui::InputInt("InstancesX", &billboardInstancesX))
			CreateQuadVAO();

		if (ImGui::InputInt("InstancesZ", &billboardInstancesZ))
			CreateQuadVAO();

		if (ImGui::InputFloat("OffsetX", &offsetX))
			UpdateBillboards();

		if (ImGui::InputFloat("OffsetZ", &offsetZ))
			UpdateBillboards();

		ImGui::TreePop();
	}
}
