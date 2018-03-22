#include "ImGui/imgui.h"
#include "Application.h"
#include "Billboard.h"
#include "ComponentCamera.h"
#include "ComponentParticleSystem.h"
#include "ModuleEditorCamera.h"
#include "ModuleScene.h"
#include "ModuleTextureManager.h"

ComponentParticleSystem::ComponentParticleSystem(GameObject* owner) : Component(owner)
{
	type = ComponentType::GRASS;
	editorInfo.idLabel = std::string(GetString(type)) + "##" + std::to_string(editorInfo.id);
	Init(20, iPoint(10, 10), 3, 10, "assets/images/rainSprite.tga", fPoint(1, 1));
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
	for (uint i = 0; i < numBillboards; ++i)
		billboards[i].SetSize(particleSize.x, particleSize.y);
	
	emissionArea = emitSize;

	fallingTime = fallTime;
	fallingHeight = fallHeight;

	textureID = App->textureManager->GetTexture(texturePath);
}

void ComponentParticleSystem::Clear()
{
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
}

void ComponentParticleSystem::UpdateSystem(const ComponentCamera& camera)
{
}

void ComponentParticleSystem::Draw()
{
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
