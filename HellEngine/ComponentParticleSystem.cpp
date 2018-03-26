#include <random>
#include <stack>
#include "ImGui/imgui.h"
#include "Application.h"
#include "Billboard.h"
#include "ComponentCamera.h"
#include "ComponentParticleSystem.h"
#include "ModuleEditorCamera.h"
#include "ModuleScene.h"
#include "ModuleTextureManager.h"
#include "ModuleTime.h"
#include "openGL.h"

ComponentParticleSystem::ComponentParticleSystem(GameObject* owner) : Component(owner)
{
	type = ComponentType::PARTICLE_SYSTEM;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	Init(1000, iPoint(10, 10), 1000, 10, "assets/images/rainSprite.tga", fPoint(1, 1));
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	Clear();
}

void ComponentParticleSystem::Init(uint maxParticles, const iPoint& emitSize, uint fallTime, float fallHeight, const char* texturePath, const fPoint& particleSize)
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

	textureID = App->textureManager->GetTexture(texturePath);
}

void ComponentParticleSystem::InitParticle(Particle& particle, uint deltaTime)
{
	particle.lifetime = fallingTime - deltaTime;
	particle.position.x = (-emissionArea.x / 2.0f) + ((float) rand() / RAND_MAX) * emissionArea.x;
	particle.position.y = fallingHeight;
	particle.position.z = ((float) rand() / RAND_MAX) * emissionArea.y;
}

void ComponentParticleSystem::Clear()
{
	App->textureManager->ReleaseTexture(textureID);

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
		particle.position += particle.velocity * (float) deltaTime;

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
		InitParticle(particle, deltaTime);
		
		nextSpawnTime += spawnInterval;
	}	
}

void ComponentParticleSystem::Draw()
{
	for (uint i = 0; i < liveParticles.size(); ++i)
	{
		uint idx = liveParticles[i];
		Particle& particle = particles[idx];
		
		glColor3f(1.0f, 1.0f, 1.0f);
		glPointSize(3.0f);
		glBegin(GL_POINTS);
		glVertex3fv(particle.position.ptr());
		glEnd();
	}
}

void ComponentParticleSystem::OnEditor()
{
	if (ImGui::CollapsingHeader(editorInfo.idLabel.c_str()))
	{
		if (OnEditorDeleteComponent())
			return;

		ImGui::Checkbox("Active", &isActive);
	}
}

int ComponentParticleSystem::MaxCountInGameObject()
{
	return 1;
}
