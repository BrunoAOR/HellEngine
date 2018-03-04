#ifndef __H_COMPONENT_AUDIOSOURCE__
#define __H_COMPONENT_AUDIOSOURCE__

#include "Component.h"

enum class AudioType { EFFECT, MUSIC };

struct AudioInfo
{
	unsigned int audioID = 0;
	AudioType audioType;
};

class ComponentAudioSource : Component
{
public:
	ComponentAudioSource(GameObject * owner);
	virtual ~ComponentAudioSource() override;

	virtual void Update() override;
	virtual void OnEditor() override;

	AudioInfo *audioInfo = nullptr;
};
#endif

