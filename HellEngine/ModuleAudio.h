#ifndef __H_MODULEAUDIO__
#define __H_MODULEAUDIO__

#include <vector>
#include "Module.h"
#include "ComponentAudioSource.h"
#include "ComponentAudioListener.h"

class ModuleAudio : public Module
{
public:

	ModuleAudio(bool startEnabled = true);
	~ModuleAudio();

	bool Load(const char *audioPath, ComponentAudioSource* source);
	bool Init();
	bool CleanUp();
	UpdateStatus PostUpdate();

	void StoreAudioSource(const ComponentAudioSource &source);
	void UnloadAudioSource(ComponentAudioSource &source);
	void UpdateActiveAudioListener(ComponentAudioListener &listener);

private:
	const char * ParseBassErrorCode(const int& bassErrorCode);
	std::string ObtainAudioExtension(const char *audioPath);

	std::vector<ComponentAudioSource*> storedAudioSources;
	ComponentAudioListener* activeAudioListener;

};

#endif /* __H_MODULEAUDIO__ */
