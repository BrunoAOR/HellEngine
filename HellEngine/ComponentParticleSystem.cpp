#include "ComponentParticleSystem.h"

ComponentParticleSystem::ComponentParticleSystem(GameObject* owner) : Component(owner)
{
}

ComponentParticleSystem::~ComponentParticleSystem()
{
}

void ComponentParticleSystem::Init(uint maxParticles, const iPoint & emitSize, uint fallingTime, float fallingHeight, const char * texturePath, const fPoint & particleSize)
{
}

void ComponentParticleSystem::Clear()
{
}

void ComponentParticleSystem::Update(uint elapsedTime, const ComponentCamera & camera)
{
}

void ComponentParticleSystem::Draw()
{
}
