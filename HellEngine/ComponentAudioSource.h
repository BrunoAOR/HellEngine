#ifndef __H_COMPONENT_AUDIOSOURCE__
#define __H_COMPONENT_AUDIOSOURCE__

#include "Component.h"

enum class AudioType { EFFECT, MUSIC };

struct AudioInfo
{
	unsigned int audioID = 0;
	AudioType audioType;
};

class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource(GameObject * owner);
	virtual ~ComponentAudioSource() override;

	virtual void Update() override;
	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

	AudioInfo *audioInfo = nullptr;
private:

	float volume;
	float pitch;
	float stereoPan;
	bool isMusicLoop;
};
#endif

