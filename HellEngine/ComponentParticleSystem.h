#ifndef __H_COMPONENT_PARTICLE_SYSTEM__
#define __H_COMPONENT_PARTICLE_SYSTEM__

#include <vector>
#include "MathGeoLib/src/Math/float3.h"
#include "Component.h"
#include "MeshInfo.h"
#include "Point.h"
#include "globals.h"

class Billboard;
class ComponentCamera;
class GameObject;
class ShaderProgram;

class ComponentParticleSystem :
	public Component
{
public:
	ComponentParticleSystem(GameObject* owner);
	virtual ~ComponentParticleSystem();

	virtual void Update() override;

	virtual void OnEditor() override;
	virtual int MaxCountInGameObject() override;

	virtual void Save(SerializableObject& obj) const override;
	virtual void Load(const SerializableObject& obj) override;

private:
	struct Particle
	{
		float3 position;
		float3 velocity;
		uint lifetime;
	};

private:
	void Init(uint maxParticles, const iPoint& emitSize, uint fallingTime,
		float fallingHeight, const fPoint& particleSize);

	void InitParticle(Particle& particle, Billboard& billboard, uint deltaTime);
	void Clear();

	void UpdateSystem();
	void UpdateVAO();
	bool DrawElements(const ComponentCamera& camera);
	
	void CreateQuadVAO();

	bool LoadTexture();

private:
	const ShaderProgram* shaderProgram = nullptr;
	bool isValid = false;

	char texturePath[256] = "";

	Particle* particles;
	uint numParticles;

	std::vector<uint> liveParticles;
	std::vector<uint> deadParticles;

	Billboard* billboards;
	uint numBillboards;

	iPoint emissionArea;
	uint fallingTime = 0;
	uint elapsedTime = 0;
	float fallingHeight = 0.0f;
	uint textureID = 0;

	uint nextSpawnTime = 0;
	uint spawnInterval = 0;

	MeshInfo particlesMeshInfo;
};

#endif

