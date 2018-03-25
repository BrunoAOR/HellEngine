#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Brofiler/include/Brofiler.h"
#include "ImGui/imgui.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/Quat.h"
#include "Application.h"
#include "ModuleAnimation.h"
#include "ModuleTime.h"
#include "SerializableArray.h"
#include "SerializableObject.h"

ModuleAnimation::ModuleAnimation()
{
}

ModuleAnimation::~ModuleAnimation()
{
}

bool ModuleAnimation::CleanUp()
{
	for (std::map<const char*, Animation*>::iterator it = animations.begin(); it != animations.end(); ++it) 
	{
		for (std::map<const char*, AnimationNode*>::iterator it2 = (*it).second->channels.begin(); it2 != (*it).second->channels.end(); ++it2) 
		{
			delete[] it2->first;
			delete[] it2->second->positions;
			delete[] it2->second->rotations;
			delete it2->second;
		}

		delete[] it->first;
		delete it->second;
	}
	animations.clear();

	for (uint i = 0; i < instances.size(); i++)
	{
		AnimationInstance* instance = instances.at(i);
	
		delete instance->next;

		delete instance;
	}
	instances.clear();

	loadedAnimationNames.clear();
	animationsNamesToPathsMap.clear();

	return true;
}

UpdateStatus ModuleAnimation::Update()
{
	for (std::vector<AnimationInstance*>::iterator it = instances.begin(); it != instances.end(); ++it)
	{
		if (*it && (*it)->active)
		{
			(*it)->time += App->time->DeltaTimeMS();
			if ((*it)->next)
			{
				(*it)->next->time += App->time->DeltaTimeMS();
				(*it)->blendTime += App->time->DeltaTimeMS();
				if ((*it)->blendTime >= (*it)->blendDuration)
					DeleteOldInstance(*it);
			}
		}
	}
	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleAnimation::Load(const char* name, const char* file)
{
	if (animationsNamesToPathsMap.count(name) != 0)
	{
		LOGGER("WARNING: ModuleAnimation - An animation with the name %s has already been loaded", name);
		return false;
	}

	const aiScene* assimpScene = aiImportFile(file, 0);

	if (assimpScene) {
		loadedAnimationNames.push_back(name);
		animationsNamesToPathsMap[name] = file;

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

			char* animationName = new char[256];
			strcpy_s(animationName, 256, name);

			animations.insert(std::make_pair(animationName, a));
		}

		aiReleaseImport(assimpScene);

		return true;
	}

	LOGGER("ERROR: ModuleAnimation - File %s not found", file);

	return false;
}

int ModuleAnimation::Play(const char* name, bool loop)
{
	if (animations.count(name) > 0) {
		Animation* anim = animations.at(name);

		AnimationInstance* instance = new AnimationInstance{ anim, 0, loop, true, nullptr, 0, 0 };

		if (holes.size() == 0) {
			instances.push_back(instance);
			return instances.size() - 1;
		}
		else {
			uint id = holes.top();
			holes.pop();
			instances.at(id) = instance;
			return id;
		}
	}

	LOGGER("Animation with name %s not found", name);
	return -1;
}

void ModuleAnimation::Stop(uint id)
{
	delete instances.at(id)->next;
	delete instances.at(id);
	instances.at(id) = nullptr;
	holes.push(id);
}

bool ModuleAnimation::BlendTo(uint id, const char* name, uint blendDuration)
{
	AnimationInstance* instance = instances.at(id);

	if (animations.count(name) > 0) {
		Animation* anim = animations.at(name);

		AnimationInstance* nextInstance = new AnimationInstance{ anim, 0, instance->loop, true, nullptr, 0, 0 };

		delete instance->next;
		instance->next = nextInstance;
		instance->blendDuration = blendDuration;

		return true;
	}

	return false;
}

void ModuleAnimation::DeleteOldInstance(AnimationInstance* instance)
{
	instance->anim = instance->next->anim;
	instance->time = instance->next->time;
	instance->loop = instance->next->loop;

	delete instance->next;

	instance->next = nullptr;
	instance->blendDuration = 0;
	instance->blendTime = 0;
}

void ModuleAnimation::ModifyAnimationLoop(uint instanceID, bool loop)
{
	if (instances.size() > 0)
	{
		AnimationInstance* instance = instances.at(instanceID);
		instance->loop = loop;
		if (instance->next)
			instance->next->loop = loop;
	}
}

void ModuleAnimation::ModifyAnimationActive(uint instanceID, bool active)
{
	AnimationInstance* instance = instances.at(instanceID);
	instance->active = active;
}

bool ModuleAnimation::GetTransform(uint id, const char* channel, float3 & position, Quat & rotation) const
{
	AnimationInstance* instance = instances.at(id);
	Animation* animation = instance->anim;
	AnimationNode* node = nullptr;

	if (animation->channels.count(channel))
		node = animation->channels.at(channel);

	if (node) {
		uint duration = animation->duration;
		uint time = instance->time;
		uint numPositions = node->numPositions;
		uint numRotations = node->numRotations;

		if (time > duration) {
			if (instance->loop) {
				instance->time -= duration;
				time = instance->time;
			}
			else {
				time = duration;
			}
		}

		float posKey = float(time * (numPositions - 1)) / duration;
		float rotKey = float(time * (numRotations - 1)) / duration;

		uint positionID = uint(posKey);
		uint rotationID = uint(rotKey);

		uint nextPositionID = (positionID + 1) % numPositions;
		uint nextRotationID = (rotationID + 1) % numRotations;

		float posLambda = posKey - float(positionID);
		float rotLambda = rotKey - float(rotationID);

		aiVector3D& pos = InterpolateVector3D(node->positions[positionID], node->positions[nextPositionID], posLambda);
		aiQuaternion& rot = InterpolateQuaternion(node->rotations[rotationID], node->rotations[nextRotationID], rotLambda);

		if (instance->next && instance->blendTime < instance->blendDuration) {
			AnimationInstance* nextAnimationInstance = instance->next;
			Animation* nextAnimationAnimation = nextAnimationInstance->anim;
			AnimationNode* nextAnimationNode = nullptr;

			if (nextAnimationAnimation->channels.count(channel))
				nextAnimationNode = nextAnimationAnimation->channels.at(channel);

			if (nextAnimationNode) {
				uint nextAnimationDuration = nextAnimationAnimation->duration;
				uint nextAnimationTime = nextAnimationInstance->time;
				uint nextAnimationNumPositions = nextAnimationNode->numPositions;
				uint nextAnimationNumRotations = nextAnimationNode->numRotations;

				if (nextAnimationTime > nextAnimationDuration) {
					if (nextAnimationInstance->loop) {
						nextAnimationInstance->time -= nextAnimationDuration;
						nextAnimationTime = nextAnimationInstance->time;
					}
					else {
						nextAnimationTime = nextAnimationDuration;
					}
				}

				float nextAnimationPosKey = float(nextAnimationTime * (nextAnimationNumPositions - 1)) / nextAnimationDuration;
				float nextAnimationRotKey = float(nextAnimationTime * (nextAnimationNumRotations - 1)) / nextAnimationDuration;

				uint nextAnimationPositionID = uint(nextAnimationPosKey);
				uint nextAnimationRotationID = uint(nextAnimationRotKey);

				uint nextAnimationNextPositionID = (nextAnimationPositionID + 1) % nextAnimationNumPositions;
				uint nextAnimationNextRotationID = (nextAnimationRotationID + 1) % nextAnimationNumRotations;

				float nextAnimationPosLambda = nextAnimationPosKey - float(nextAnimationPositionID);
				float nextAnimationRotLambda = nextAnimationRotKey - float(nextAnimationRotationID);

				const aiVector3D& nextAnimationPos = InterpolateVector3D(
					nextAnimationNode->positions[nextAnimationPositionID], 
					nextAnimationNode->positions[nextAnimationNextPositionID],
					nextAnimationPosLambda
				);

				const aiQuaternion& nextAnimationRot = InterpolateQuaternion(
					nextAnimationNode->rotations[nextAnimationRotationID],
					nextAnimationNode->rotations[nextAnimationNextRotationID], 
					nextAnimationRotLambda);

				float blendLambda = float(instance->blendTime) / instance->blendDuration;

				aiVector3D& finalPos = InterpolateVector3D(pos, nextAnimationPos, blendLambda);
				aiQuaternion& finalRot = InterpolateQuaternion(rot, nextAnimationRot, blendLambda);

				pos = finalPos;
				rot = finalRot;
			}
		}

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

void ModuleAnimation::OnEditorAnimationWindow(float mainMenuBarHeight, bool* pOpen)
{
	static char animationPath[256] = "";
	static char animationName[256] = "";
	static std::string loadMessage = "";

	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(450, 600));
	ImGui::Begin("Animation options", pOpen, ImGuiWindowFlags_NoCollapse);

	ImGui::InputText("Animation path", animationPath, 256);
	ImGui::InputText("Animation name", animationName, 256);

	if (ImGui::Button("Load"))
	{
		if (animationsNamesToPathsMap.count(animationName) != 0)
		{
			loadMessage = "Animation name already in use";
		}
		else
		{
			bool pathInUse = false;
			for (std::unordered_map<std::string, std::string>::iterator it = animationsNamesToPathsMap.begin(); it != animationsNamesToPathsMap.end(); ++it)
			{
				if (strcmp(it->second.c_str(), animationPath) == 0)
				{
					loadMessage = "Animation already loaded with name " + it->first;
					pathInUse = true;
					break;
				}
			}

			if (!pathInUse)
			{
				if (App->animation->Load(animationName, animationPath))
				{
					animationPath[0] = '\0';
					animationName[0] = '\0';
					loadMessage = "Animation loaded correctly.";
				}
				else
					loadMessage = "Animation not found.";
			}
		}
	}

	ImGui::Text(loadMessage.c_str());

	ImGui::Separator();
	ImGui::Text("Loaded animations:");

	ImGui::Indent();
	if (loadedAnimationNames.empty())
	{
		ImGui::Text("No animations loaded.");
	}
	else
	{
		for (const std::string& name : loadedAnimationNames)
		{
			ImGui::Text(name.c_str());
			ImGui::Indent();
			ImGui::TextWrapped((std::string("From path: ") + animationsNamesToPathsMap[name]).c_str());
			ImGui::Unindent();
		}
	}
	ImGui::Unindent();

	ImGui::End();
}

void ModuleAnimation::Save(SerializableObject& obj)
{
	SerializableObject animationsObject = obj.BuildSerializableObject("Animations");
	animationsObject.AddVectorString("AnimationNames", loadedAnimationNames);
	
	SerializableArray animationsArray = animationsObject.BuildSerializableArray("NamesToPaths");
	for (std::unordered_map<std::string, std::string>::iterator it = animationsNamesToPathsMap.begin(); it != animationsNamesToPathsMap.end(); ++it)
	{
		SerializableObject pair = animationsArray.BuildSerializableObject();
		pair.AddString("Name", it->first);
		pair.AddString("Path", it->second);
	}
}

void ModuleAnimation::Load(const SerializableObject& obj)
{
	/* Delete any previously loaded animations */
	CleanUp();

	/* Load the loadedAnimationNames and animationsNamesToPathsMap from the SerializableObject into temporary objects*/
	std::vector<std::string> namesTemp;
	std::unordered_map<std::string, std::string> nameToPathTemp;

	SerializableObject animationsObject = obj.GetSerializableObject("Animations");
	namesTemp = animationsObject.GetVectorString("AnimationNames");
	
	SerializableArray animationsArray = animationsObject.GetSerializableArray("NamesToPaths");
	uint arraySize = animationsArray.Size();
	for (uint i = 0; i < arraySize; ++i)
	{
		SerializableObject pair = animationsArray.GetSerializableObject(i);
		nameToPathTemp[pair.GetString("Name")] = pair.GetString("Path");
	}

	/* Load the animations from the temp objects */
	for (const std::string& animationName : namesTemp)
	{
		bool success = Load(animationName.c_str(), nameToPathTemp[animationName].c_str());
		assert(success);
	}
}

aiVector3D ModuleAnimation::InterpolateVector3D(const aiVector3D& first, const aiVector3D& second, float lambda) const
{
	return first * (1.0f - lambda) + second * lambda;
}

aiQuaternion ModuleAnimation::InterpolateQuaternion(const aiQuaternion& first, const aiQuaternion& second, float lambda) const
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
