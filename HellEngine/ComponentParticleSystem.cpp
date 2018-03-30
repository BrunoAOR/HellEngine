#include <random>
#include <stack>
#include "ImGui/imgui.h"
#include "Application.h"
#include "Billboard.h"
#include "ComponentCamera.h"
#include "ComponentParticleSystem.h"
#include "ModuleEditorCamera.h"
#include "ModuleScene.h"
#include "ModuleShaderManager.h"
#include "ModuleTextureManager.h"
#include "ModuleTime.h"
#include "ShaderProgram.h"
#include "openGL.h"

ComponentParticleSystem::ComponentParticleSystem(GameObject* owner) : Component(owner)
{
	type = ComponentType::PARTICLE_SYSTEM;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	Init(500, iPoint(10, 10), 2000, 10, fPoint(0.2f, 0.2f));
	shaderProgram = App->shaderManager->GetShaderProgram("assets/shaders/defaultShader.vert", "assets/shaders/defaultShader.frag");
	assert(shaderProgram);
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	Clear();
}

void ComponentParticleSystem::Init(uint maxParticles, const iPoint& emitSize, uint fallTime, float fallHeight, const fPoint& particleSize)
{
	numParticles = numBillboards = maxParticles;

	particles = new Particle[numParticles];
	billboards = new Billboard[numBillboards];

	liveParticles.reserve(numParticles);
	deadParticles.reserve(numParticles);

	for (uint i = 0; i < numParticles; ++i)
	{
		particles[i].lifetime = fallTime;
		particles[i].velocity = float3(0, -fallHeight / fallTime, 0);
		deadParticles.push_back(i);

		billboards[i].SetSize(particleSize.x, particleSize.y);
	}
	
	emissionArea = emitSize;

	fallingTime = fallTime;
	fallingHeight = fallHeight;

	spawnInterval = fallingTime / numParticles;
}

void ComponentParticleSystem::InitParticle(Particle& particle, Billboard& billboard, uint deltaTime)
{
	particle.lifetime = fallingTime - deltaTime;
	particle.position.x = (-emissionArea.x / 2.0f) + ((float) rand() / RAND_MAX) * emissionArea.x;
	particle.position.y = fallingHeight;
	particle.position.z = (((float) rand() / RAND_MAX) * emissionArea.y) - (emissionArea.y / 2);
	billboard.SetPosition(particle.position);
}

void ComponentParticleSystem::Clear()
{
	App->shaderManager->ReleaseShaderProgram(shaderProgram);
	shaderProgram = nullptr;

	if (textureID != 0)
		App->textureManager->ReleaseTexture(textureID);

	textureID = 0;

	delete [] particles;
	particles = nullptr;

	delete[] billboards;
	billboards = nullptr;
}

void ComponentParticleSystem::Update()
{
	ComponentCamera* activeCamera = App->scene->GetActiveGameCamera();
	if (!activeCamera)
		activeCamera = App->editorCamera->camera;

	UpdateSystem(*activeCamera);

	Draw();
}

void ComponentParticleSystem::UpdateSystem(const ComponentCamera& camera)
{
	uint deltaTime = App->time->DeltaTimeMS();
	elapsedTime += deltaTime;

	if (elapsedTime > fallingTime)
	{
		elapsedTime -= fallingTime;
		nextSpawnTime = 0;
	}

	uint particlesToKill = 0;
	for (uint i = 0; i < liveParticles.size(); ++i)
	{
		uint idx = liveParticles[i];
		Particle& particle = particles[idx];
		Billboard& billboard = billboards[idx];
		particle.position += particle.velocity * (float) deltaTime;
		billboard.SetPosition(particle.position);

		if (particle.lifetime <= deltaTime)
			++particlesToKill;
		else
			particle.lifetime -= deltaTime;

	}

	if (particlesToKill > 0)
	{
		for (uint i = 0; i < particlesToKill; ++i)
			deadParticles.push_back(liveParticles[i]);

		liveParticles.erase(liveParticles.begin(), liveParticles.begin() + particlesToKill);
	}	

	while (elapsedTime > nextSpawnTime)
	{
		assert(deadParticles.size() > 0);
		uint idx = deadParticles.back();
		deadParticles.pop_back();

		liveParticles.push_back(idx);

		Particle& particle = particles[idx];
		Billboard& billboard = billboards[idx];
		InitParticle(particle, billboard, deltaTime);
		
		nextSpawnTime += spawnInterval;
	}	

	CreateQuadVAO();
}

void ComponentParticleSystem::Draw()
{
	if (isValid)
	{
		const uint allVertCount = 6;
		const uint uniqueVertCount = 4;
		const uint aliveBillboards = liveParticles.size();

		GLfloat* newVertQuadData = new GLfloat[uniqueVertCount * 3 * aliveBillboards];
		
		for (uint i = 0; i < aliveBillboards; ++i)
		{
			uint idx = liveParticles[i];
			Billboard& billboard = billboards[idx];

			ComponentCamera* activeCamera = App->scene->GetActiveGameCamera();
			if (!activeCamera)
				activeCamera = App->editorCamera->camera;

			const Quad& quad = billboard.ComputeQuad(activeCamera);

			/* Quad vertices, color, and UV */
			for (uint j = 0; j < uniqueVertCount; ++j)
			{
				newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 0] = quad.vertices[j].x;
				newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 1] = quad.vertices[j].y;
				newVertQuadData[i * uniqueVertCount * 3 + (j * 3) + 2] = quad.vertices[j].z;
			}
		}

		glBindVertexArray(particlesMeshInfo.vao);
		glBindBuffer(GL_ARRAY_BUFFER, particlesMeshInfo.vbo);
		float* oldQuadData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		int j = 0;
		for (uint i = 0; i < uniqueVertCount * 8 * aliveBillboards; ++i)
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

void ComponentParticleSystem::CreateQuadVAO()
{
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
	const uint aliveBillboards = liveParticles.size();

	GLfloat* uniqueQuadData = new GLfloat[uniqueVertCount * 8 * aliveBillboards];
	GLuint* verticesOrder = new GLuint[allVertCount * aliveBillboards];

	for (uint i = 0; i < aliveBillboards; ++i)
	{
		uint idx = liveParticles[i];
		Billboard& billboard = billboards[idx];

		ComponentCamera* activeCamera = App->scene->GetActiveGameCamera();
		if (!activeCamera)
			activeCamera = App->editorCamera->camera;

		const Quad& quad = billboard.ComputeQuad(activeCamera);

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

	particlesMeshInfo.name = "Quad";
	particlesMeshInfo.elementsCount = allVertCount * aliveBillboards;

	glGenVertexArrays(1, &particlesMeshInfo.vao);
	glGenBuffers(1, &particlesMeshInfo.vbo);
	glGenBuffers(1, &particlesMeshInfo.ebo);

	glBindVertexArray(particlesMeshInfo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, particlesMeshInfo.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uniqueVertCount * 8 * aliveBillboards, uniqueQuadData, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE); /* Can be unbound, since the vertex information is stored in the VAO throught the VertexAttribPointers */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particlesMeshInfo.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * particlesMeshInfo.elementsCount, verticesOrder, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE); /* Can be unbound AFTER unbinding the VAO, since the VAO stores information about the bound EBO */

	delete[] uniqueQuadData;
	delete[] verticesOrder;
}

bool ComponentParticleSystem::DrawElements()
{
	if (isValid)
	{
		shaderProgram->Activate();

		ComponentCamera* activeCamera = App->scene->GetActiveGameCamera();
		if (!activeCamera)
			activeCamera = App->editorCamera->camera;
		
		float3 position = activeCamera->GetPosition3();
		float4x4 modelMatrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			position.x, position.y, position.z, 1
		);

		shaderProgram->UpdateMatrixUniforms(modelMatrix.ptr(), App->editorCamera->camera->GetViewMatrix(), App->editorCamera->camera->GetProjectionMatrix());

		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(particlesMeshInfo.vao);
		glDrawElements(GL_TRIANGLES, particlesMeshInfo.elementsCount, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(GL_NONE);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		shaderProgram->Deactivate();
		return true;
	}

	return false;
}

bool ComponentParticleSystem::LoadTexture()
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

void ComponentParticleSystem::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::Checkbox("Active", &isActive);

		ImGui::InputText("Texture path", texturePath, 256);
		if (ImGui::Button("Load Texture"))
			LoadTexture();
	}
}

int ComponentParticleSystem::MaxCountInGameObject()
{
	return 1;
}
