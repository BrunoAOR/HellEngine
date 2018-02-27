#ifndef __H_MODULE_ANIMATION__
#define __H_MODULE_ANIMATION__

#include <list>
#include <map>
#include "Assimp/include/assimp/cimport.h"
#include "Module.h"
#include "globals.h"

struct AnimationNode {
	const char* name;
	/* No vector because sizes will not change */
	aiVector3D* positions = nullptr;
	uint numPositions = 0;
	aiQuaternion* rotations = nullptr;
	uint numRotations = 0;
};

struct Animation {
	uint duration = 0;
	AnimationNode* channels = nullptr;
	uint numChannels = 0;
};

struct AnimationInstance {
	Animation* anim = nullptr;
	uint time = 0;
	bool loop = true;

	AnimationInstance* next = nullptr;
	uint blendDuration = 0;
	uint blendTime = 0;
};

class ModuleAnimation : public Module {

	std::map<const char*, Animation*> animations;
	std::vector<AnimationInstance*> instances;
	std::list<uint> holes;

public:

	ModuleAnimation();
	~ModuleAnimation();

	bool Load(const char* name, const char* file);
	bool CleanUp();
	UpdateStatus Update();

	uint Play(const char* name, bool loop = true);
	void Stop(uint id);
	void BlendTo(uint id, const char* name, uint blendTime);

	bool GetTransform(uint id, const char* channel, float3& position, Quat& rotation) const;

};

#endif