#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Application.h"
#include "ModuleAnimation.h"

ModuleAnimation::ModuleAnimation()
{
}

ModuleAnimation::~ModuleAnimation()
{
}

bool ModuleAnimation::Load(const char * name, const char * file)
{
	/*
	const aiScene* assimpScene = aiImportFile(file, 0);

	if (assimpScene) {
		for (uint i = 0; i < assimpScene->mNumAnimations; i++) {
			aiAnimation* anim = assimpScene->mAnimations[i];
			Animation a;
			a.duration = uint(anim->mDuration);
			a.numChannels = anim->mNumChannels;
			a.channels = new AnimationNode[a.numChannels];
			for (uint j = 0; j < anim->mNumChannels; j++) {
				AnimationNode* node = new AnimationNode{ anim->mChannels[j]->mNodeName, anim->mChannels[j]->mPositionKeys->mValue, anim->mChannels[j]->mNumPositionKeys, anim->mChannels[j]->mRotationKeys->mValue, anim->mChannels[j]->mNumRotationKeys };
				a.channels[j] = node;
			}
		}

		return true;
	}
	*/

	return false;
}

bool ModuleAnimation::CleanUp()
{
	for (std::map<aiString, Animation*>::iterator it = animations.begin(); it != animations.end(); ++it)
		delete it->second;

	for (uint i = 0; i < instances.size(); i++)
		delete instances.at(i);

	return true;
}
