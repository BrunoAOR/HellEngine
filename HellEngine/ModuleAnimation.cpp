#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Brofiler/include/Brofiler.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/Quat.h"
#include "Application.h"
#include "ModuleAnimation.h"
#include "ModuleTime.h"

ModuleAnimation::ModuleAnimation()
{
	Load("Idle", "assets/models/ArmyPilot/Animations/ArmyPilot_Idle.fbx");
}

ModuleAnimation::~ModuleAnimation()
{
}

bool ModuleAnimation::Load(const char * name, const char * file)
{
	const aiScene* assimpScene = aiImportFile(file, 0);

	if (assimpScene) {
		for (uint i = 0; i < assimpScene->mNumAnimations; i++) {
			aiAnimation* anim = assimpScene->mAnimations[i];

			Animation* a = new Animation();
			a->duration = uint(anim->mDuration / anim->mTicksPerSecond) * 1000;
			/*
			a->numChannels = anim->mNumChannels;
			a->channels = new AnimationNode[a->numChannels];
			*/
			for (uint j = 0; j < anim->mNumChannels; j++) {
				/*
				a->channels[j].name = anim->mChannels[j]->mNodeName.C_Str();
				a->channels[j].positions = &anim->mChannels[j]->mPositionKeys->mValue;
				a->channels[j].numPositions = anim->mChannels[j]->mNumPositionKeys;
				a->channels[j].rotations = &anim->mChannels[j]->mRotationKeys->mValue;
				a->channels[j].numRotations = anim->mChannels[j]->mNumRotationKeys;
				*/
				AnimationNode* node = new AnimationNode();

				uint length = anim->mChannels[j]->mNodeName.length;
				char* auxName = new char[length - 1];
				memcpy_s(auxName, length, anim->mChannels[j]->mNodeName.C_Str(), length);
				auxName[length] = '\0';
				const char* channelName = auxName;

				node->numPositions = anim->mChannels[j]->mNumPositionKeys;
				node->numRotations = anim->mChannels[j]->mNumRotationKeys;

				node->positions = new aiVector3D[node->numPositions];
				node->rotations = new aiQuaternion[node->numRotations];

				for (uint n = 0; n < node->numPositions; n++) {
					node->positions[n] = anim->mChannels[j]->mPositionKeys[n].mValue;
					node->rotations[n] = anim->mChannels[j]->mRotationKeys[n].mValue;
				}

				a->channels.insert(std::make_pair(channelName, node));
			}

			animations.insert(std::make_pair(name, a));
		}

		aiReleaseImport(assimpScene);

		return true;
	}

	aiReleaseImport(assimpScene);

	return false;
}

bool ModuleAnimation::CleanUp()
{
	for (std::map<const char*, Animation*>::iterator it = animations.begin(); it != animations.end(); ++it) {
		for (std::map<const char*, AnimationNode*>::iterator it2 = (*it).second->channels.begin(); it2 != (*it).second->channels.end(); ++it2) {
			delete it2->second->positions;
			delete it2->second->rotations;
			delete it2->second;
		}

		delete it->second;
	}

	for (uint i = 0; i < instances.size(); i++)
		delete instances.at(i);

	return true;
}

UpdateStatus ModuleAnimation::Update()
{
	for (std::vector<AnimationInstance*>::iterator it = instances.begin(); it != instances.end(); ++it)
		(*it)->time += App->time->DeltaTimeMS();

	return UpdateStatus::UPDATE_CONTINUE;
}


uint ModuleAnimation::Play(const char * name, bool loop)
{
	Animation* anim = animations.at(name);

	AnimationInstance* instance = new AnimationInstance{ anim, 0, loop, nullptr, 0, 0 };

	if (holes.size() == 0) {
		instances.push_back(instance);
		return instances.size() - 1;
	}
	else {
		uint id = holes.back();
		holes.pop_back();
		instances.at(id) = instance;
		return id;
	}
}

void ModuleAnimation::Stop(uint id)
{
	delete instances.at(id);
	instances.at(id) = nullptr;
	holes.push_back(id);

}

bool ModuleAnimation::GetTransform(uint id, const char * channel, float3 & position, Quat & rotation) const
{
	AnimationInstance* instance = instances.at(id);
	Animation* animation = instance->anim;
	AnimationNode* node = nullptr;

	if (animation->channels.count(channel))
		node = animation->channels.at(channel);

	/*
	for (uint i = 0; i < animation->numChannels; i++) {
		if (strcmp(animation->channels[i].name, channel) == 0) {
			node = &animation->channels[i];
			break;
		}
	}
	*/

	if (node) {
		uint duration = animation->duration;
		uint time = instance->time;

		if (time > duration)
			int a = 2;

		float timeFraction = (float)fmod(time, duration) / duration;

		uint numPositions = node->numPositions;
		uint numRotations = node->numRotations;

		uint positionID = uint(floor(timeFraction * numPositions));
		uint rotationID = uint(floor(timeFraction * numRotations));

		const aiVector3D& pos = node->positions[positionID];
		const aiQuaternion& rot = node->rotations[rotationID];

		position.x = (float)pos.x;
		position.y = (float)pos.y;
		position.z = (float)pos.z;

		rotation.x = (float)rot.x;
		rotation.y = (float)rot.y;
		rotation.z = (float)rot.z;
		rotation.w = (float)rot.w;

		return true;
	}
	else {
		return false;
	}
}
