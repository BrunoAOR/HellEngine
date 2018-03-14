#ifndef __H_COMPONENT_AUDIOSOURCE__
#define __H_COMPONENT_AUDIOSOURCE__

#include "Component.h"

enum class AudioType { EFFECT, MUSIC };
enum class AudioState { NOT_LOADED, LOADED, WAITING_TO_PLAY, WAITING_TO_STOP, WAITING_TO_PAUSE, CURRENTLY_PAUSED, CURRENTLY_PLAYED, CURRENTLY_STOPPED};

struct AudioInfo
{
	unsigned long audioID = 0;
	AudioType audioType;
};

class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource(GameObject * owner);
	virtual ~ComponentAudioSource() override;

	virtual void Update() override;
	virtual void OnEditor() override;

	bool Play();
	bool Pause();
	bool Stop();

	float GetVolume() const;
	float GetPitch() const;
	float GetStereoMono() const;
	float GetStereoPan() const;
	float GetDopplerLevel() const;
	float GetMinDistance() const;
	float GetMaxDistance() const;
	bool GetIsLopping() const;

	void SetVolume(float volumeValue);
	void SetPitch(float pitchValue);
	void SetStereoMono(float stereoOrMonoValue);
	void SetStereoPan(float panValue);
	void SetDopplerLevel(float dopplerLevelValue);
	void SetMinDistance(float minDistanceValue);
	void SetMaxDistance(float maxDistanceValue);

	void SetPlayBackState(int playBackStateValue);

	AudioState GetCurrentState() const;
	void SetCurrentState(AudioState audioStateValue);

	AudioState GetPreviousState() const;

	void SetPreviousState(AudioState previousAudioSourceStateValue);

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

	AudioInfo *audioInfo = nullptr;
private:

	float volume;
	float pitch;
	float stereoPan;
	float stereoMono;
	float reverbZone;
	float dopplerLevel;
	int rollOff;
	float minDistance = 10;
	float maxDistance = 500;
	bool isMusicLoop;

	bool loadMessage = false;
	bool audioAlreadyExists = false;
	bool loadedCorrectly = false;
	char audioPath[256] = "";

	//Test
	int playbackState;

	AudioState currentAudioSourceState = AudioState::NOT_LOADED;
	AudioState previousAudioSourceState = AudioState::NOT_LOADED;
};
#endif

