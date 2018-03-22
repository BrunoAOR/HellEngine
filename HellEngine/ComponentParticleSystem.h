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

class ComponentParticleSystem :
	public Component
{
public:
	ComponentParticleSystem(GameObject* owner);
	virtual ~ComponentParticleSystem();

	void Init(uint maxParticles, const iPoint& emitSize, uint fallingTime, 
		float fallingHeight, const char* texturePath, const fPoint& particleSize);

	virtual void Update() override;
	void UpdateSystem(const ComponentCamera& camera);

	void Clear();

	void Draw();

	virtual void OnEditor() override;
	virtual int MaxCountInGameObject() override;

private:

	struct Particle
	{
		float3 position;
		float3 velocity;
		uint lifetime;
	};

	Particle* particles;
	uint numParticles;

	std::vector<uint> particlesAlive;
	std::vector<uint> particlesDead;

	Billboard* billboards;
	uint numBillboards;

	iPoint emissionArea;
	uint fallingTime = 0;
	uint elapsedTime = 0;
	float fallingHeight = 0.0f;
	uint textureID = 0;

	MeshInfo particlesMeshInfo;
};

#endif

