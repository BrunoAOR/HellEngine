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

			for (uint j = 0; j < anim->mNumChannels; j++) {
				AnimationNode* node = new AnimationNode();

				uint length = anim->mChannels[j]->mNodeName.length;
				char* auxName = new char[length + 1];
				memcpy_s(auxName, length, anim->mChannels[j]->mNodeName.C_Str(), length);
				auxName[length] = '\0';
				const char* channelName = auxName;

				node->numPositions = anim->mChannels[j]->mNumPositionKeys;
				node->numRotations = anim->mChannels[j]->mNumRotationKeys;

				node->positions = new aiVector3D[node->numPositions];
				node->rotations = new aiQuaternion[node->numRotations];

				for (uint n = 0; n < node->numPositions; n++) {
					node->positions[n].x = anim->mChannels[j]->mPositionKeys[n].mValue.x;
					node->positions[n].y = anim->mChannels[j]->mPositionKeys[n].mValue.y;
					node->positions[n].z = anim->mChannels[j]->mPositionKeys[n].mValue.z;

					node->rotations[n].x = anim->mChannels[j]->mRotationKeys[n].mValue.x;
					node->rotations[n].y = anim->mChannels[j]->mRotationKeys[n].mValue.y;
					node->rotations[n].z = anim->mChannels[j]->mRotationKeys[n].mValue.z;
					node->rotations[n].w = anim->mChannels[j]->mRotationKeys[n].mValue.w;
				}

				a->channels.insert(std::make_pair(channelName, node));
			}

			animations.insert(std::make_pair(name, a));
		}

		aiReleaseImport(assimpScene);

		return true;
	}

	LOGGER("File %s not found", file);

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
	for (std::vector<AnimationInstance*>::iterator it = instances.begin(); it != instances.end(); ++it) {
		if (*it) {
			(*it)->time += App->time->DeltaTimeMS();
		}
	}

	return UpdateStatus::UPDATE_CONTINUE;
}


int ModuleAnimation::Play(const char * name, bool loop)
{
	if (animations.count(name) > 0) {
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

	LOGGER("Animation with name %s not found", name);
	return -1;
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
		uint numPositions = node->numPositions;
		uint numRotations = node->numRotations;

		if (time > duration) {
			if (instance->loop) {
				instance->time = 0;
				time = 0;
			}
			else {
				time = duration;
			}
		}

		float posKey = float(time * (numPositions - 1) / duration);
		float rotKey = float(time * (numRotations - 1) / duration);

		uint positionID = uint(posKey);
		uint rotationID = uint(rotKey);

		uint nextPositionID = (positionID + 1) % numPositions;
		uint nextRotationID = (positionID + 1) % numPositions;

		float posLambda = posKey - float(positionID);
		float rotLambda = rotKey - float(rotationID);

		const aiVector3D& pos = InterpolateVector3D(node->positions[positionID], node->positions[nextPositionID], posLambda);
		const aiQuaternion& rot = InterpolateQuaternion(node->rotations[rotationID], node->rotations[nextRotationID], rotLambda);

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

aiVector3D ModuleAnimation::InterpolateVector3D(const aiVector3D & first, const aiVector3D & second, float lambda) const
{
	return first * (1.0f - lambda) + second * lambda;
}

aiQuaternion ModuleAnimation::InterpolateQuaternion(const aiQuaternion & first, const aiQuaternion & second, float lambda) const
{
	aiQuaternion result;

	float dot = first.x * second.x + first.y * second.y + first.z * second.z + first.w * second.w;

	if (dot >= 0.0f) {
		result.x = first.x * (1.0f - lambda) + second.x * lambda;
		result.y = first.y * (1.0f - lambda) + second.y * lambda;
		result.z = first.z * (1.0f - lambda) + second.z * lambda;
		result.w = first.w * (1.0f - lambda) + second.w * lambda;
	}
	else {
		result.x = first.x * (1.0f - lambda) + second.x * -lambda;
		result.y = first.y * (1.0f - lambda) + second.y * -lambda;
		result.z = first.z * (1.0f - lambda) + second.z * -lambda;
		result.w = first.w * (1.0f - lambda) + second.w * -lambda;
	}

	result.Normalize();

	return result;
}
